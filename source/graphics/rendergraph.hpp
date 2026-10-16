#pragma once
#include "core/containers.h"
#include "core/primitivetypes.h"
#include "core/utility.h"

namespace Khan
{
	enum ResourceState;
	class Resource;
	class Buffer;
	class BufferView;
	class Texture;
	class TextureView;
	class Renderer;
	class RenderPass;
	class RenderDevice;
	class TransientResourceManager;
	struct BufferDesc;
	struct BufferViewDesc;
	struct TextureDesc;
	struct TextureViewDesc;

	// RenderContext::BeginRecording(RenderPass& pass)
	// it will then query the render graph, which will be located in RenderDevice, by using the pass to get the resources that need to be transitioned
	// same will be done in RenderContext::EndRecording(RenderPass& pass)

	class RenderGraph
	{
	public:
		RenderGraph(TransientResourceManager* transientResourceManager);

		void AddPass(RenderPass& renderPass);

		void EnableAsyncCompute(bool enable);
		void EnableDMA(bool enable);

		Buffer* CreateManagedResource(const BufferDesc& desc);
		Texture* CreateManagedResource(const TextureDesc& desc);
		BufferView* DeclareResourceDependency(Buffer* buffer, const BufferViewDesc& desc, ResourceState startState, bool keepContent = false);
		BufferView* DeclareResourceDependency(Buffer* buffer, const BufferViewDesc& desc, ResourceState startState, ResourceState endState, bool keepContent = false);
		TextureView* DeclareResourceDependency(Texture* texture, const TextureViewDesc& desc, ResourceState startState, bool keepContent = false);
		TextureView* DeclareResourceDependency(Texture* texture, const TextureViewDesc& desc, ResourceState startState, ResourceState endState, bool keepContent = false);

		void Setup(Renderer& renderer);
		void Compile();
		void Execute(Renderer& renderer);
		void Reset();

		class Node
		{
		public:
			Node(RenderPass& pass);

			inline const THashSet<uint64>& ReadSubresources() const { return m_ReadSubresources; }
			inline const THashSet<uint64>& WrittenSubresources() const { return m_WrittenSubresources; }
			inline const THashSet<uint64>& ReadAndWritten() const { return m_ReadAndWrittenSubresources; }
			inline const THashSet<uint64>& AllResources() const { return m_AllResources; }
			inline const TArray<const Node*>& NodesToSyncWith() const { return m_NodesToSyncWith; }
			inline uint64 GlobalExecutionIndex() const { return m_GlobalExecutionIndex; }
			inline uint64 DependencyLevelIndex() const { return m_DependencyLevelIndex; }
			inline uint64 LocalToDependencyLevelExecutionIndex() const { return m_LocalToDependencyLevelExecutionIndex; }
			inline uint64 LocalToQueueExecutionIndex() const { return m_LocalToQueueExecutionIndex; }
			inline bool IsSyncSignalRequired() const { return m_SyncSignalRequired; }
			inline const TArray<BufferView*>& AllBuffers() const { return m_AllBuffers; }
			inline const TArray<TextureView*>& AllTextures() const { return m_AllTextures; }

			void AddReadDependency(uint64 resourceID, uint32 firstSubresourceIndex, uint32 subresourceCount);
			void AddWriteDependency(uint64 resourceID, uint32 firstSubresourceIndex, uint32 subresourceCount); 

			bool HasDependency(uint64 resourceID, uint32 subresourceIndex) const;
			bool HasDependency(uint64 subresourceID) const;
			bool HasAnyDependencies() const;

			TPair<ResourceState, uint64> GetBarrierDataForResource(const Resource& resource);
			
			uint64 m_ExecutionQueueIndex = 0;

		private:
			friend RenderGraph;

			inline static constexpr uint64 InvalidSynchronizationIndex = std::numeric_limits<uint64>::max();

			// TODO:
			//void EnsureSingleWriteDependency(uint64 resourceID, uint32 subresourceIndex);
			void Clear();

			RenderPass& m_Pass;

			uint64 m_GlobalExecutionIndex = 0;
			uint64 m_DependencyLevelIndex = 0;
			uint64 m_LocalToDependencyLevelExecutionIndex = 0;
			uint64 m_LocalToQueueExecutionIndex = 0;
			uint64 m_IndexInUnorderedList = 0;

			THashSet<uint64> m_ReadSubresources;
			THashSet<uint64> m_WrittenSubresources;
			THashSet<uint64> m_ReadAndWrittenSubresources;
			THashSet<uint64> m_AllResources;

			TArray<BufferView*> m_AllBuffers;
			TArray<TextureView*> m_AllTextures;
			THashMap<Resource*, uint64> m_ResourceToIdMap;
			THashMap<uint64, TPair<ResourceState, ResourceState>> m_ResourceStateTransition;
			THashMap<uint64, uint64> m_ResourceQueueTransfer;

			TArray<uint64> m_SynchronizationIndexSet;
			TArray<const Node*> m_NodesToSyncWith;
			bool m_SyncSignalRequired = false;
		};

		class DependencyLevel
		{
		public:
			inline const TList<Node*>& Nodes() const { return m_Nodes; }
			inline const TArray<const Node*>& NodesPerQueue(uint64 queueIndex) const { return m_NodesPerQueue[queueIndex]; }
			inline const THashSet<uint64>& QueuesInvoledInCrossQueueResourceReads() const { return m_QueuesInvoledInCrossQueueResourceReads; }
			inline const THashSet<uint64>& SubresourcesReadByMultipleQueues() const { return m_SubresourcesReadByMultipleQueues; }
			inline uint64 LevelIndex() const { return m_LevelIndex; }

		private:
			friend RenderGraph;

			void AddNode(Node* node);
			Node* RemoveNode(TList<Node*>::iterator it);

			uint64 m_LevelIndex = 0;
			TList<Node*> m_Nodes;
			TArray<TArray<const Node*>> m_NodesPerQueue;

			// Storage for queues that read at least one common resource. Resource state transitions
			// for such queues need to be handled differently.
			THashSet<uint64> m_QueuesInvoledInCrossQueueResourceReads;
			THashSet<uint64> m_SubresourcesReadByMultipleQueues;
		};

		static uint64 ConstructSubresourceID(uint64 resourceID, uint32 subresourceIndex);
		static TPair<uint64, uint32> DecodeSubresourceName(uint64 name);

		uint64 NodeCountForQueue(uint64 queueIndex) const;

		Node* GetPassNode(const RenderPass* pass);

	private:
		struct SyncCoverage
		{
			const Node* m_NodeToSyncWith = nullptr;
			uint64 m_NodeToSyncWithIndex = 0;
			TArray<uint64> m_SyncedQueueIndices;
		};

		//void EnsureRenderPassUniqueness(const RenderPass* pass);
		void BuildAdjacencyLists();
		void DepthFirstSearch(uint64 nodeIndex, TArray<bool>& visited, TArray<bool>& onStack, bool& isCyclic);
		void TopologicalSort();
		void BuildDependencyLevels();
		void FinalizeDependencyLevels();
		void CullRedundantSynchronizations();
		void ResolveTransitionsAndTransfers();

		TransientResourceManager* m_TransientResourceManager;

		TMap<const RenderPass*, Node*> m_PassToNodeMap;
		TPair<RenderPass*, Node*> m_ActivePassNode;
		uint32 m_CreatedResourceIndex;

		TArray<Node> m_PassNodes;
		TArray<TArray<uint64>> m_AdjacencyLists;
		TArray<DependencyLevel> m_DependencyLevels;

		THashMap<uint64, uint64> m_QueueNodeCounters;
		TArray<Node*> m_TopologicallySortedNodes;
		TArray<Node*> m_NodesInGlobalExecutionOrder;
		THashMap<uint64, const Node*> m_WrittenSubresourceToPassMap;
		uint64 m_DetectedQueueCount = 1;
		TArray<TArray<const Node*>> m_NodesPerQueue;

		static const bool ms_ResourceStateToReadAccess[];
	};
}