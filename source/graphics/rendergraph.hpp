#pragma once

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

			inline const std::unordered_set<uint64_t>& ReadSubresources() const { return m_ReadSubresources; }
			inline const std::unordered_set<uint64_t>& WrittenSubresources() const { return m_WrittenSubresources; }
			inline const std::unordered_set<uint64_t>& ReadAndWritten() const { return m_ReadAndWrittenSubresources; }
			inline const std::unordered_set<uint64_t>& AllResources() const { return m_AllResources; }
			inline const std::vector<const Node*>& NodesToSyncWith() const { return m_NodesToSyncWith; }
			inline uint64_t GlobalExecutionIndex() const { return m_GlobalExecutionIndex; }
			inline uint64_t DependencyLevelIndex() const { return m_DependencyLevelIndex; }
			inline uint64_t LocalToDependencyLevelExecutionIndex() const { return m_LocalToDependencyLevelExecutionIndex; }
			inline uint64_t LocalToQueueExecutionIndex() const { return m_LocalToQueueExecutionIndex; }
			inline bool IsSyncSignalRequired() const { return m_SyncSignalRequired; }
			inline const std::vector<BufferView*>& AllBuffers() const { return m_AllBuffers; }
			inline const std::vector<TextureView*>& AllTextures() const { return m_AllTextures; }

			void AddReadDependency(uint64_t resourceID, uint32_t firstSubresourceIndex, uint32_t subresourceCount);
			void AddWriteDependency(uint64_t resourceID, uint32_t firstSubresourceIndex, uint32_t subresourceCount); 

			bool HasDependency(uint64_t resourceID, uint32_t subresourceIndex) const;
			bool HasDependency(uint64_t subresourceID) const;
			bool HasAnyDependencies() const;

			std::pair<ResourceState, uint64_t> GetBarrierDataForResource(const Resource& resource);
			
			uint64_t m_ExecutionQueueIndex = 0;

		private:
			friend RenderGraph;

			inline static constexpr uint64_t InvalidSynchronizationIndex = std::numeric_limits<uint64_t>::max();

			// TODO:
			//void EnsureSingleWriteDependency(uint64_t resourceID, uint32_t subresourceIndex);
			void Clear();

			RenderPass& m_Pass;

			uint64_t m_GlobalExecutionIndex = 0;
			uint64_t m_DependencyLevelIndex = 0;
			uint64_t m_LocalToDependencyLevelExecutionIndex = 0;
			uint64_t m_LocalToQueueExecutionIndex = 0;
			uint64_t m_IndexInUnorderedList = 0;

			std::unordered_set<uint64_t> m_ReadSubresources;
			std::unordered_set<uint64_t> m_WrittenSubresources;
			std::unordered_set<uint64_t> m_ReadAndWrittenSubresources;
			std::unordered_set<uint64_t> m_AllResources;

			std::vector<BufferView*> m_AllBuffers;
			std::vector<TextureView*> m_AllTextures;
			std::unordered_map<Resource*, uint64_t> m_ResourceToIdMap;
			std::unordered_map<uint64_t, std::pair<ResourceState, ResourceState>> m_ResourceStateTransition;
			std::unordered_map<uint64_t, uint64_t> m_ResourceQueueTransfer;

			std::vector<uint64_t> m_SynchronizationIndexSet;
			std::vector<const Node*> m_NodesToSyncWith;
			bool m_SyncSignalRequired = false;
		};

		class DependencyLevel
		{
		public:
			inline const std::list<Node*>& Nodes() const { return m_Nodes; }
			inline const std::vector<const Node*>& NodesPerQueue(uint64_t queueIndex) const { return m_NodesPerQueue[queueIndex]; }
			inline const std::unordered_set<uint64_t>& QueuesInvoledInCrossQueueResourceReads() const { return m_QueuesInvoledInCrossQueueResourceReads; }
			inline const std::unordered_set<uint64_t>& SubresourcesReadByMultipleQueues() const { return m_SubresourcesReadByMultipleQueues; }
			inline uint64_t LevelIndex() const { return m_LevelIndex; }

		private:
			friend RenderGraph;

			void AddNode(Node* node);
			Node* RemoveNode(std::list<Node*>::iterator it);

			uint64_t m_LevelIndex = 0;
			std::list<Node*> m_Nodes;
			std::vector<std::vector<const Node*>> m_NodesPerQueue;

			// Storage for queues that read at least one common resource. Resource state transitions
			// for such queues need to be handled differently.
			std::unordered_set<uint64_t> m_QueuesInvoledInCrossQueueResourceReads;
			std::unordered_set<uint64_t> m_SubresourcesReadByMultipleQueues;
		};

		static uint64_t ConstructSubresourceID(uint64_t resourceID, uint32_t subresourceIndex);
		static std::pair<uint64_t, uint32_t> DecodeSubresourceName(uint64_t name);

		uint64_t NodeCountForQueue(uint64_t queueIndex) const;

		Node* GetPassNode(const RenderPass* pass);

	private:
		struct SyncCoverage
		{
			const Node* m_NodeToSyncWith = nullptr;
			uint64_t m_NodeToSyncWithIndex = 0;
			std::vector<uint64_t> m_SyncedQueueIndices;
		};

		//void EnsureRenderPassUniqueness(const RenderPass* pass);
		void BuildAdjacencyLists();
		void DepthFirstSearch(uint64_t nodeIndex, std::vector<bool>& visited, std::vector<bool>& onStack, bool& isCyclic);
		void TopologicalSort();
		void BuildDependencyLevels();
		void FinalizeDependencyLevels();
		void CullRedundantSynchronizations();
		void ResolveTransitionsAndTransfers();

		TransientResourceManager* m_TransientResourceManager;

		std::map<const RenderPass*, Node*> m_PassToNodeMap;
		std::pair<RenderPass*, Node*> m_ActivePassNode;
		uint32_t m_CreatedResourceIndex;

		std::vector<Node> m_PassNodes;
		std::vector<std::vector<uint64_t>> m_AdjacencyLists;
		std::vector<DependencyLevel> m_DependencyLevels;

		std::unordered_map<uint64_t, uint64_t> m_QueueNodeCounters;
		std::vector<Node*> m_TopologicallySortedNodes;
		std::vector<Node*> m_NodesInGlobalExecutionOrder;
		std::unordered_map<uint64_t, const Node*> m_WrittenSubresourceToPassMap;
		uint64_t m_DetectedQueueCount = 1;
		std::vector<std::vector<const Node*>> m_NodesPerQueue;

		static const bool ms_ResourceStateToReadAccess[];
	};
}