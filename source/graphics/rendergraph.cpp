#include "graphics/precomp.h"
#include "graphics/rendergraph.hpp"
#include "graphics/renderer.hpp"
#include "graphics/renderpass.hpp"
#include "graphics/rendertask.hpp"
#include "graphics/hal/buffer.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/transientresourcemanager.hpp"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkandevice.hpp"
#endif

namespace Khan
{
	const bool RenderGraph::ms_ResourceStateToReadAccess[] = {
			false,	// ResourceState_Undefined
			false,	// ResourceState_General
			true,	// ResourceState_CopySource
			false,	// ResourceState_CopyDestination
			true,	// ResourceState_VertexBuffer
			true,	// ResourceState_IndexBuffer
			true,	// ResourceState_IndirectArgument
			true,	// ResourceState_ConstantBuffer
			false,	// ResourceState_UnorderedAccess
			true,	// ResourceState_NonPixelShaderAccess
			true,	// ResourceState_PixelShaderAccess
			true,	// ResourceState_AnyShaderAccess
			false,	// ResourceState_StreamOut
			false,	// ResourceState_RenderTarget
			false,	// ResourceState_DepthWrite
			true,	// ResourceState_DepthRead
			false,	// ResourceState_StencilWrite
			true,	// ResourceState_StencilRead
			false,	// ResourceState_DepthWriteStencilWrite
			true,	// ResourceState_DepthReadStencilRead
			false,	// ResourceState_DepthReadStencilWrite
			false,	// ResourceState_DepthWriteStencilRead
			true	// ResourceState_Present
	};

	inline uint64_t RenderGraph::ConstructSubresourceID(uint64_t resourceID, uint32_t subresourceIndex)
	{
		return resourceID & (~0ull << 32) ^ (resourceID << 32) | subresourceIndex;
	}

	inline std::pair<uint64_t, uint32_t> RenderGraph::DecodeSubresourceName(uint64_t name)
	{
		return { 0, 0 };
	}

	uint64_t RenderGraph::NodeCountForQueue(uint64_t queueIndex) const
	{
		auto countIt = m_QueueNodeCounters.find(queueIndex);
		return countIt != m_QueueNodeCounters.end() ? countIt->second : 0;
	}

	RenderGraph::Node* RenderGraph::GetPassNode(const RenderPass* pass)
	{
		return m_PassToNodeMap[pass];
	}

	RenderGraph::RenderGraph(TransientResourceManager* transientResourceManager)
		: m_TransientResourceManager(transientResourceManager)
	{
	}

	void RenderGraph::AddPass(RenderPass& pass)
	{
		Node& insertedNode = m_PassNodes.emplace_back(Node(pass));
		insertedNode.m_IndexInUnorderedList = m_PassNodes.size() - 1;
	}

	void RenderGraph::EnableAsyncCompute(bool enable)
	{
		m_ActivePassNode.second->m_ExecutionQueueIndex = enable ? QueueType_Compute : QueueType_Graphics;
	}

	Buffer* RenderGraph::CreateManagedResource(const BufferDesc& desc)
	{
		return m_TransientResourceManager->FindOrCreateBuffer(m_ActivePassNode.first, desc);
	}

	Texture* RenderGraph::CreateManagedResource(const TextureDesc& desc)
	{
		return m_TransientResourceManager->FindOrCreateTexture(m_ActivePassNode.first, desc);
	}

	BufferView* RenderGraph::DeclareResourceDependency(Buffer* buffer, const BufferViewDesc& desc, ResourceState startState, bool keepContent)
	{
		return DeclareResourceDependency(buffer, desc, startState, startState, keepContent);
	}

	BufferView* RenderGraph::DeclareResourceDependency(Buffer* buffer, const BufferViewDesc& desc, ResourceState startState, ResourceState endState, bool keepContent)
	{
		BufferView* view = m_TransientResourceManager->FindOrCreateBufferView(buffer, desc);		
		uint64_t resourceID = pair_hash{}(std::pair(buffer, desc));

		Node* node = m_ActivePassNode.second;
		node->m_AllBuffers.push_back(view);
		node->m_ResourceToIdMap.insert({ &view->GetBuffer(), resourceID });
		node->m_ResourceStateTransition.insert({ resourceID, { startState, endState } });

		if (ms_ResourceStateToReadAccess[startState])
		{
			node->AddReadDependency(resourceID, desc.m_Offset, desc.m_Range);
		}
		else
		{
			node->AddWriteDependency(resourceID, desc.m_Offset, desc.m_Range);
			if (keepContent)
			{
				node->AddReadDependency(resourceID, desc.m_Offset, desc.m_Range);
			}
		}

		return view;
	}

	TextureView* RenderGraph::DeclareResourceDependency(Texture* texture, const TextureViewDesc& desc, ResourceState startState, bool keepContent)
	{
		return DeclareResourceDependency(texture, desc, startState, startState, keepContent);
	}

	TextureView* RenderGraph::DeclareResourceDependency(Texture* texture, const TextureViewDesc& desc, ResourceState startState, ResourceState endState, bool keepContent)
	{
		TextureView* view = m_TransientResourceManager->FindOrCreateTextureView(texture, desc);
		uint64_t resourceID = pair_hash{}(std::pair(texture, desc));

		Node* node = m_ActivePassNode.second;
		node->m_AllTextures.push_back(view);
		node->m_ResourceToIdMap.insert({ &view->GetTexture(), resourceID });
		node->m_ResourceStateTransition.insert({ resourceID, { startState, endState } });

		if (ms_ResourceStateToReadAccess[startState])
		{
			node->AddReadDependency(resourceID, desc.m_BaseArrayLayer, desc.m_LayerCount);
		}
		else
		{
			node->AddWriteDependency(resourceID, desc.m_BaseArrayLayer, desc.m_LayerCount);
			if (keepContent)
			{
				node->AddReadDependency(resourceID, desc.m_BaseArrayLayer, desc.m_LayerCount);
			}
		}

		return view;
	}

	void RenderGraph::Setup(Renderer& renderer)
	{
		for (auto& it : m_PassNodes)
		{
			m_ActivePassNode.first = &it.m_Pass;
			m_ActivePassNode.second = &it;
			it.m_Pass.Setup(*this, renderer);
			m_PassToNodeMap.insert(m_ActivePassNode);
		}
	}

	void RenderGraph::Compile()
	{
		BuildAdjacencyLists();
		TopologicalSort();
		BuildDependencyLevels();
		FinalizeDependencyLevels();
		CullRedundantSynchronizations();
		ResolveTransitionsAndTransfers();
	}

	inline void RenderGraph::BuildAdjacencyLists()
	{
		m_AdjacencyLists.resize(m_PassNodes.size());

		for (uint64_t nodeIdx = 0; nodeIdx < m_PassNodes.size(); ++nodeIdx)
		{
			Node& node = m_PassNodes[nodeIdx];

			if (!node.HasAnyDependencies())
			{
				continue;
			}

			std::vector<uint64_t>& adjacentNodeIndices = m_AdjacencyLists[nodeIdx];

			//for (uint64_t otherNodeIdx = 0; otherNodeIdx < m_PassNodes.size(); ++otherNodeIdx)
			for (uint64_t otherNodeIdx = nodeIdx + 1; otherNodeIdx < m_PassNodes.size(); ++otherNodeIdx)
			{
				// Do not check dependencies on itself
				//if (nodeIdx == otherNodeIdx) continue;

				Node& otherNode = m_PassNodes[otherNodeIdx];

				auto establishAdjacency = [&](uint64_t otherNodeReadResource) -> bool
				{
					// If other node reads a subresource written by the current node, then it depends on current node and is an adjacent dependency
					bool otherNodeDependsOnCurrentNode =
						node.m_WrittenSubresources.find(otherNodeReadResource) != node.m_WrittenSubresources.end();

					if (otherNodeDependsOnCurrentNode)
					{
						adjacentNodeIndices.push_back(otherNodeIdx);

						if (node.m_ExecutionQueueIndex != otherNode.m_ExecutionQueueIndex)
						{
							node.m_SyncSignalRequired = true;
							otherNode.m_NodesToSyncWith.push_back(&node);
						}

						return true;
					}

					return false;
				};

				for (uint64_t otherNodeReadResource : otherNode.m_ReadSubresources)
				{
					if (establishAdjacency(otherNodeReadResource)) break;
				}

				/*for (uint64_t otherNodeReadResource : otherNode.mAliasedSubresources)
				{
					if (establishAdjacency(otherNodeReadResource)) break;
				}*/
			}
		}
	}

	void RenderGraph::DepthFirstSearch(uint64_t nodeIndex, std::vector<bool>& visited, std::vector<bool>& onStack, bool& isCyclic)
	{
		if (isCyclic)
			return;

		visited[nodeIndex] = true;
		onStack[nodeIndex] = true;

		uint64_t adjacencyListIndex = m_PassNodes[nodeIndex].m_IndexInUnorderedList;

		for (uint64_t neighbour : m_AdjacencyLists[adjacencyListIndex])
		{
			if (visited[neighbour] && onStack[neighbour])
			{
				isCyclic = true;
				return;
			}

			if (!visited[neighbour])
			{
				DepthFirstSearch(neighbour, visited, onStack, isCyclic);
			}
		}

		onStack[nodeIndex] = false;
		m_TopologicallySortedNodes.push_back(&m_PassNodes[nodeIndex]);
	}

	inline void RenderGraph::TopologicalSort()
	{
		std::vector<bool> visitedNodes(m_PassNodes.size(), false);
		std::vector<bool> onStackNodes(m_PassNodes.size(), false);

		bool isCyclic = false;

		for (uint64_t nodeIndex = 0; nodeIndex < m_PassNodes.size(); ++nodeIndex)
		{
			const Node& node = m_PassNodes[nodeIndex];

			// Visited nodes and nodes without outputs are not processed
			if (!visitedNodes[nodeIndex] && node.HasAnyDependencies())
			{
				DepthFirstSearch(nodeIndex, visitedNodes, onStackNodes, isCyclic);
				KH_ASSERT(!isCyclic, "Detected cyclic dependency in pass.");
			}
		}

		std::reverse(m_TopologicallySortedNodes.begin(), m_TopologicallySortedNodes.end());
	}

	inline void RenderGraph::BuildDependencyLevels()
	{
		std::vector<uint64_t> longestDistances(m_PassNodes.size(), 0);

		uint64_t dependencyLevelCount = 1;

		// Perform longest node distance search
		for (uint64_t nodeIndex = 0; nodeIndex < m_TopologicallySortedNodes.size(); ++nodeIndex)
		{
			uint64_t originalIndex = m_TopologicallySortedNodes[nodeIndex]->m_IndexInUnorderedList;
			uint64_t adjacencyListIndex = originalIndex;

			for (uint64_t adjacentNodeIndex : m_AdjacencyLists[adjacencyListIndex])
			{
				if (longestDistances[adjacentNodeIndex] < longestDistances[originalIndex] + 1)
				{
					uint64_t newLongestDistance = longestDistances[originalIndex] + 1;
					longestDistances[adjacentNodeIndex] = newLongestDistance;
					dependencyLevelCount = std::max(newLongestDistance + 1, dependencyLevelCount);
				}
			}
		}

		m_DependencyLevels.resize(dependencyLevelCount);
		m_DetectedQueueCount = 1;

		// Dispatch nodes to corresponding dependency levels.
		for (auto nodeIndex = 0; nodeIndex < m_TopologicallySortedNodes.size(); ++nodeIndex)
		{
			Node* node = m_TopologicallySortedNodes[nodeIndex];
			uint64_t levelIndex = longestDistances[node->m_IndexInUnorderedList];
			DependencyLevel& dependencyLevel = m_DependencyLevels[levelIndex];
			dependencyLevel.m_LevelIndex = levelIndex;
			dependencyLevel.AddNode(node);
			node->m_DependencyLevelIndex = levelIndex;
			m_DetectedQueueCount = std::max(m_DetectedQueueCount, node->m_ExecutionQueueIndex + 1);
		}
	}

	
	inline void RenderGraph::FinalizeDependencyLevels()
	{
		uint32_t globalExecutionIndex = 0;

		m_NodesInGlobalExecutionOrder.resize(m_TopologicallySortedNodes.size(), nullptr);
		m_NodesPerQueue.resize(m_DetectedQueueCount);
		std::vector<const Node*> perQueuePreviousNodes(m_DetectedQueueCount, nullptr);

		for (DependencyLevel& dependencyLevel : m_DependencyLevels)
		{
			uint64_t localExecutionIndex = 0;

			std::unordered_map<uint64_t, std::unordered_set<uint64_t>> resourceReadingQueueTracker;
			dependencyLevel.m_NodesPerQueue.resize(m_DetectedQueueCount);

			for (Node* node : dependencyLevel.m_Nodes)
			{
				// Track which resource is read by which queue in this dependency level
				for (uint64_t subresourceName : node->ReadSubresources())
				{
					resourceReadingQueueTracker[subresourceName].insert(node->m_ExecutionQueueIndex);
				}

				// Associate written subresource with render pass that writes to it for quick access when needed
				for (uint64_t subresourceName : node->WrittenSubresources())
				{
					m_WrittenSubresourceToPassMap[subresourceName] = node;
				}

				node->m_GlobalExecutionIndex = globalExecutionIndex;
				node->m_LocalToDependencyLevelExecutionIndex = localExecutionIndex;
				node->m_LocalToQueueExecutionIndex = m_QueueNodeCounters[node->m_ExecutionQueueIndex]++;

				m_NodesInGlobalExecutionOrder[globalExecutionIndex] = node;

				dependencyLevel.m_NodesPerQueue[node->m_ExecutionQueueIndex].push_back(node);
				m_NodesPerQueue[node->m_ExecutionQueueIndex].push_back(node);

				// Add previous node on that queue as a dependency for sync optimization later
				if (perQueuePreviousNodes[node->m_ExecutionQueueIndex])
				{
					node->m_NodesToSyncWith.push_back(perQueuePreviousNodes[node->m_ExecutionQueueIndex]);
				}

				perQueuePreviousNodes[node->m_ExecutionQueueIndex] = node;

				//for (uint64_t resourceName : node->AllResources())
				//{
				//	auto timelineIt = mResourceUsageTimelines.find(resourceName);
				//	bool timelineExists = timelineIt != mResourceUsageTimelines.end();

				//	if (timelineExists)
				//	{
				//		// Update "end" 
				//		timelineIt->second.second = node->GlobalExecutionIndex();
				//	}
				//	else {
				//		// Create "start"
				//		auto& timeline = mResourceUsageTimelines[resourceName];
				//		timeline.first = node->GlobalExecutionIndex();
				//		timeline.second = node->GlobalExecutionIndex();
				//	}
				//}

				//// Track first RT-using node to sync BVH builds with
				//if (node->UsesRayTracing && !mFirstNodesThatUseRayTracing[node->ExecutionQueueIndex])
				//{
				//	mFirstNodesThatUseRayTracing[node->ExecutionQueueIndex] = node;
				//}

				localExecutionIndex++;
				globalExecutionIndex++;
			}

			// Record queue indices that are detected to read common resources
			for (auto& [subresourceName, queueIndices] : resourceReadingQueueTracker)
			{
				// If resource is read by more than one queue
				if (queueIndices.size() > 1)
				{
					for (uint64_t queueIndex : queueIndices)
					{
						dependencyLevel.m_QueuesInvoledInCrossQueueResourceReads.insert(queueIndex);
						dependencyLevel.m_SubresourcesReadByMultipleQueues.insert(subresourceName);
					}
				}
			}
		}
	}
 
	inline void RenderGraph::CullRedundantSynchronizations()
	{
		// Initialize synchronization index sets
		for (Node& node : m_PassNodes)
		{
			node.m_SynchronizationIndexSet.resize(m_DetectedQueueCount, Node::InvalidSynchronizationIndex);
		}

		for (DependencyLevel& dependencyLevel : m_DependencyLevels)
		{
			// First pass: find closest nodes to sync with, compute initial SSIS (sufficient synchronization index set)
			for (Node* node : dependencyLevel.m_Nodes)
			{
				// Closest node to sync with on each queue
				std::vector<const Node*> closestNodesToSyncWith{ m_DetectedQueueCount, nullptr };

				// Find closest dependencies from other queues for the current node
				for (const Node* dependencyNode : node->m_NodesToSyncWith)
				{
					const Node* closestNode = closestNodesToSyncWith[dependencyNode->m_ExecutionQueueIndex];

					if (!closestNode || dependencyNode->LocalToQueueExecutionIndex() > closestNode->LocalToQueueExecutionIndex())
					{
						closestNodesToSyncWith[dependencyNode->m_ExecutionQueueIndex] = dependencyNode;
					}
				}

				// Get rid of nodes to sync that may have had redundancies
				node->m_NodesToSyncWith.clear();

				// Compute initial SSIS
				for (uint64_t queueIdx = 0; queueIdx < m_DetectedQueueCount; ++queueIdx)
				{
					const Node* closestNode = closestNodesToSyncWith[queueIdx];

					if (!closestNode)
					{
						// If we do not have a closest node to sync with on another queue (queueIdx),
						// we need to use SSIS value for that queue from the previous node on this node's queue (closestNodesToSyncWith[node->ExecutionQueueIndex])
						// to correctly propagate SSIS values for all queues through the graph and do not lose them
						const Node* previousNodeOnNodesQueue = closestNodesToSyncWith[node->m_ExecutionQueueIndex];

						// Previous node can be null if we're dealing with first node in the queue
						if (previousNodeOnNodesQueue)
						{
							uint64_t syncIndexForOtherQueueFromPreviousNode = previousNodeOnNodesQueue->m_SynchronizationIndexSet[queueIdx];
							node->m_SynchronizationIndexSet[queueIdx] = syncIndexForOtherQueueFromPreviousNode;
						}
					}
					else
					{
						// Update SSIS using closest nodes' indices
						if (closestNode->m_ExecutionQueueIndex != node->m_ExecutionQueueIndex)
							node->m_SynchronizationIndexSet[closestNode->m_ExecutionQueueIndex] = closestNode->LocalToQueueExecutionIndex();

						// Store only closest nodes to sync with
						node->m_NodesToSyncWith.push_back(closestNode);
					}
				}

				// Use node's execution index as synchronization index on its own queue
				node->m_SynchronizationIndexSet[node->m_ExecutionQueueIndex] = node->LocalToQueueExecutionIndex();
			}

			// Second pass: cull redundant dependencies by searching for indirect synchronizations
			for (Node* node : dependencyLevel.m_Nodes)
			{
				// Keep track of queues we still need to sync with
				std::unordered_set<uint64_t> queueToSyncWithIndices;

				// Store nodes and queue syncs they cover
				std::vector<SyncCoverage> syncCoverageArray;

				// Final optimized list of nodes without redundant dependencies
				std::vector<const Node*> optimalNodesToSyncWith;

				for (const Node* nodeToSyncWith : node->m_NodesToSyncWith)
				{
					queueToSyncWithIndices.insert(nodeToSyncWith->m_ExecutionQueueIndex);
				}

				while (!queueToSyncWithIndices.empty())
				{
					uint64_t maxNumberOfSyncsCoveredBySingleNode = 0;

					for (auto dependencyNodeIdx = 0u; dependencyNodeIdx < node->m_NodesToSyncWith.size(); ++dependencyNodeIdx)
					{
						const Node* dependencyNode = node->m_NodesToSyncWith[dependencyNodeIdx];

						// Take a dependency node and check how many queues we would sync with 
						// if we would only sync with this one node. We very well may encounter a case
						// where by synchronizing with just one node we will sync with more then one queue
						// or even all of them through indirect synchronizations, 
						// which will make other synchronizations previously detected for this node redundant.

						std::vector<uint64_t> syncedQueueIndices;

						for (uint64_t queueIndex : queueToSyncWithIndices)
						{
							uint64_t currentNodeDesiredSyncIndex = node->m_SynchronizationIndexSet[queueIndex];
							uint64_t dependencyNodeSyncIndex = dependencyNode->m_SynchronizationIndexSet[queueIndex];

							KH_ASSERT(currentNodeDesiredSyncIndex != Node::InvalidSynchronizationIndex,
								"Bug! Node that wants to sync with some queue must have a valid sync index for that queue.");

							if (queueIndex == node->m_ExecutionQueueIndex)
							{
								currentNodeDesiredSyncIndex -= 1;
							}

							if (dependencyNodeSyncIndex != Node::InvalidSynchronizationIndex &&
								dependencyNodeSyncIndex >= currentNodeDesiredSyncIndex)
							{
								syncedQueueIndices.push_back(queueIndex);
							}
						}

						syncCoverageArray.emplace_back(SyncCoverage{ dependencyNode, dependencyNodeIdx, syncedQueueIndices });
						maxNumberOfSyncsCoveredBySingleNode = std::max(maxNumberOfSyncsCoveredBySingleNode, syncedQueueIndices.size());
					}

					for (const SyncCoverage& syncCoverage : syncCoverageArray)
					{
						auto coveredSyncCount = syncCoverage.m_SyncedQueueIndices.size();

						if (coveredSyncCount >= maxNumberOfSyncsCoveredBySingleNode)
						{
							// Optimal list of synchronizations should not contain nodes from the same queue,
							// because work on the same queue is synchronized automatically and implicitly
							if (syncCoverage.m_NodeToSyncWith->m_ExecutionQueueIndex != node->m_ExecutionQueueIndex)
							{
								optimalNodesToSyncWith.push_back(syncCoverage.m_NodeToSyncWith);

								// Update SSIS
								auto& index = node->m_SynchronizationIndexSet[syncCoverage.m_NodeToSyncWith->m_ExecutionQueueIndex];
								index = std::max(index, node->m_SynchronizationIndexSet[syncCoverage.m_NodeToSyncWith->m_ExecutionQueueIndex]);
							}

							// Remove covered queues from the list of queues we need to sync with
							for (uint64_t syncedQueueIndex : syncCoverage.m_SyncedQueueIndices)
							{
								queueToSyncWithIndices.erase(syncedQueueIndex);
							}
						}
					}

					// Remove nodes that we synced with from the original list. Reverse iterating to avoid index invalidation.
					for (auto syncCoverageIt = syncCoverageArray.rbegin(); syncCoverageIt != syncCoverageArray.rend(); ++syncCoverageIt)
					{
						node->m_NodesToSyncWith.erase(node->m_NodesToSyncWith.begin() + syncCoverageIt->m_NodeToSyncWithIndex);
					}
				}

				// Finally, assign an optimal list of nodes to sync with to the current node
				node->m_NodesToSyncWith = optimalNodesToSyncWith;
			}
		}
	}

	inline void RenderGraph::ResolveTransitionsAndTransfers()
	{
		for (auto it = m_TopologicallySortedNodes.rbegin(); it != m_TopologicallySortedNodes.rend(); ++it)
		{
			Node* node = *it;

			uint64_t prevNodeIdx = node->m_LocalToQueueExecutionIndex - 1;
			if (prevNodeIdx != Node::InvalidSynchronizationIndex)
			{
				Node* prevNode = const_cast<Node*>(m_NodesPerQueue[node->m_ExecutionQueueIndex][prevNodeIdx]);

				for (auto resourceIt : node->m_ResourceToIdMap)
				{
					auto [resource, resourceID] = resourceIt;

					auto& prevNodeResStateDecl = prevNode->m_ResourceStateTransition.find(resourceID);
					if (prevNodeResStateDecl != prevNode->m_ResourceStateTransition.end())
					{
						ResourceState prevNodeResEndState = prevNodeResStateDecl->second.second;
						resource->SetState(prevNodeResEndState);
						resource->SetQueue(prevNode->m_Pass.GetExecutionQueue());
					}
				}
			}
			else
			{
				for (auto resourceIt : node->m_ResourceToIdMap)
				{
					auto [resource, resourceID] = resourceIt;
					resource->SetState(ResourceState_Undefined);
					resource->SetQueue(node->m_Pass.GetExecutionQueue());
				}
			}
			
			for (const Node* prevConstNode : node->m_NodesToSyncWith)
			{
				Node* prevNode = const_cast<Node*>(prevConstNode);

				for (auto resourceIt : node->m_ResourceToIdMap)
				{
					auto [resource, resourceID] = resourceIt;

					auto& prevNodeResStateDecl = prevNode->m_ResourceStateTransition.find(resourceID);
					if (prevNodeResStateDecl != prevNode->m_ResourceStateTransition.end())
					{
						ResourceState& prevNodeResEndState = prevNodeResStateDecl->second.second;
						resource->SetState(prevNodeResEndState);
						auto& currNodeResStateDecl = node->m_ResourceStateTransition.find(resourceID);
						ResourceState currNodeResStartState = currNodeResStateDecl->second.first;
						prevNodeResEndState = currNodeResStartState;

						resource->SetQueue(prevNode->m_Pass.GetExecutionQueue());
						prevNode->m_ResourceQueueTransfer.insert({ resourceID, node->m_ExecutionQueueIndex });
					}
				}
			}
		}
	}

	void RenderGraph::Execute(Renderer& renderer)
	{
		const std::vector<RenderContext*> contexts = m_TransientResourceManager->GetDevice().GetContexts();
		uint64_t ctxCount = contexts.size();
		ThreadPool& threadPool = renderer.GetThreadPool();

		uint64_t currentCtxIdx = 0;
		for (Node* node : m_NodesInGlobalExecutionOrder)
		{
			RenderTask task(node->m_Pass, *contexts[currentCtxIdx], renderer);
			threadPool.AddJob(task);
			currentCtxIdx = (currentCtxIdx + 1) % ctxCount;
		}

		threadPool.Wait();
	}

	void RenderGraph::Reset()
	{
		//m_GlobalWriteDependencyRegistry.clear();
		m_WrittenSubresourceToPassMap.clear();
		m_DependencyLevels.clear();
		//m_ResourceUsageTimelines.clear();
		m_QueueNodeCounters.clear();
		m_TopologicallySortedNodes.clear();
		m_NodesInGlobalExecutionOrder.clear();
		m_AdjacencyLists.clear();
		m_DetectedQueueCount = 1;
		m_NodesPerQueue.clear();
		//m_FirstNodesThatUseRayTracing.clear();
		m_PassNodes.clear();
		m_PassToNodeMap.clear();
		m_DetectedQueueCount = 0;
	}

	RenderGraph::Node::Node(RenderPass& pass)
		: m_Pass(pass)
	{
	}

	void RenderGraph::Node::AddReadDependency(uint64_t resourceID, uint32_t firstSubresourceIndex, uint32_t subresourceCount)
	{
		for (uint32_t i = 0; i < subresourceCount; ++i)
		{
			uint64_t subresourceID = ConstructSubresourceID(resourceID, firstSubresourceIndex + i);
			m_ReadSubresources.insert(subresourceID);
			m_ReadAndWrittenSubresources.insert(subresourceID);
			m_AllResources.insert(resourceID);
		}
	}
	void RenderGraph::Node::AddWriteDependency(uint64_t resourceID, uint32_t firstSubresourceIndex, uint32_t subresourceCount)
	{
		for (uint32_t i = 0; i < subresourceCount; ++i)
		{
			uint64_t subresourceID = ConstructSubresourceID(resourceID, firstSubresourceIndex + i);
			m_WrittenSubresources.insert(subresourceID);
			m_ReadAndWrittenSubresources.insert(subresourceID);
			m_AllResources.insert(resourceID);
		}
	}

	bool RenderGraph::Node::HasDependency(uint64_t resourceID, uint32_t subresourceIndex) const
	{
		return HasDependency(ConstructSubresourceID(resourceID, subresourceIndex));
	}

	bool RenderGraph::Node::HasDependency(uint64_t subresourceID) const
	{
		return m_ReadAndWrittenSubresources.find(subresourceID) != m_ReadAndWrittenSubresources.end();
	}

	bool RenderGraph::Node::HasAnyDependencies() const
	{
		return !m_ReadAndWrittenSubresources.empty();
	}

	std::pair<ResourceState, uint64_t> RenderGraph::Node::GetBarrierDataForResource(const Resource& resource)
	{
		uint64_t resourceID = m_ResourceToIdMap[&const_cast<Resource&>(resource)];

		ResourceState state = m_ResourceStateTransition[resourceID].second;
		uint64_t queue = m_ExecutionQueueIndex;
		auto it = m_ResourceQueueTransfer.find(resourceID);
		if (it != m_ResourceQueueTransfer.end())
		{
			queue = it->second;
		}

		return { state, queue };
	}

	void RenderGraph::Node::Clear()
	{
		m_ReadSubresources.clear();
		m_WrittenSubresources.clear();
		m_ReadAndWrittenSubresources.clear();
		m_AllResources.clear();
		//m_AliasedSubresources.clear();
		m_NodesToSyncWith.clear();
		m_SynchronizationIndexSet.clear();
		m_DependencyLevelIndex = 0;
		m_SyncSignalRequired = false;
		m_ExecutionQueueIndex = QueueType_Graphics;
		//UsesRayTracing = false;
		m_GlobalExecutionIndex = 0;
		m_LocalToDependencyLevelExecutionIndex = 0;
		m_AllBuffers.clear();
		m_AllTextures.clear();
		m_ResourceToIdMap.clear();
		m_ResourceStateTransition.clear();
		m_ResourceQueueTransfer.clear();
	}

	// TODO:
	/*void RenderPassGraph::Node::EnsureSingleWriteDependency(SubresourceName name)
	{
		auto [resourceName, subresourceIndex] = DecodeSubresourceName(name);

		assert_format(mWriteDependencyRegistry->find(name) == mWriteDependencyRegistry->end(),
			"Resource ", resourceName.ToString(), ", subresource ", subresourceIndex, " already has a write dependency. ",
			"Use Aliases to perform multiple writes into the same resource.");

		mWriteDependencyRegistry->insert(name);
	}*/

	void RenderGraph::DependencyLevel::AddNode(Node* node)
	{
		m_Nodes.push_back(node);
	}

	RenderGraph::Node* RenderGraph::DependencyLevel::RemoveNode(std::list<Node*>::iterator it)
	{
		Node* node = *it;
		m_Nodes.erase(it);
		return node;
	}
}