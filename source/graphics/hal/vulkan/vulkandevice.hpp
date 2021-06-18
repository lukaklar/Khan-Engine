#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/queuetype.hpp"
#include "graphics/rendergraph.hpp"
#include "graphics/hal/vulkan/vulkandevicememorymanager.hpp"
#include "graphics/hal/vulkan/vulkanphysicalrenderpassmanager.hpp"
#include "graphics/hal/vulkan/vulkanpipelinestatemanager.hpp"
#include "graphics/hal/vulkan/vulkansemaphoreallocator.hpp"
#include "graphics/hal/vulkan/vulkantransientresourcemanager.hpp"
#include "graphics/hal/vulkan/vulkanuniformbufferallocator.hpp"

namespace Khan
{
	struct BufferViewDesc;
	struct ShaderDesc;
	struct TextureViewDesc;
	class BufferView;
	class DisplayAdapter;
	class Shader;
	class TextureView;

	class RenderDevice
	{
		friend class RenderContext;
		friend class Swapchain;
	public:
		RenderDevice(const DisplayAdapter& adapter);
		~RenderDevice();

		inline VkDevice VulkanDevice() const { return m_Device; }

		inline const DisplayAdapter& GetAdapter() const { return m_Adapter; }

		inline const std::vector<RenderContext*>& GetContexts() const { return m_Contexts; }

		RenderGraph& GetRenderGraph() { return m_RenderGraph; }

		Buffer* CreateBuffer(const BufferDesc& desc);
		BufferView* CreateBufferView(Buffer* buffer, const BufferViewDesc& desc);

		Texture* CreateTexture(const TextureDesc& desc);
		TextureView* CreateTextureView(Texture* texture, const TextureViewDesc& desc);

		Shader* CreateShader(const ShaderDesc& desc);

		PhysicalRenderPass* CreatePhysicalRenderPass(const PhysicalRenderPassDescription& desc);

		RenderPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineDescription& desc);
		RenderPipelineState* CreateComputePipelineState(const ComputePipelineDescription& desc);

		void DestroyBuffer(Buffer* buffer);
		void DestroyBufferView(BufferView* view);

		void DestroyTexture(Texture* texture);
		void DestroyTextureView(TextureView* view);

		void DestroyShader(Shader* shader);

		inline uint64_t FrameNumber() const { return m_FrameCounter; }

		void OnFlip(uint32_t frameIndex);

	private:
		void SubmitCommands(VkCommandBuffer commandBuffer, const RenderGraph::Node* rgSubmitInfo);
		void FlushCommands();

		VkDevice m_Device;
		VkQueue m_CommandQueues[QueueType_Count];
		std::vector<RenderContext*> m_Contexts;
		const DisplayAdapter& m_Adapter;
		uint64_t m_FrameCounter;

		VulkanDeviceMemoryManager m_MemoryManager;
		VulkanTransientResourceManager m_TransientResourceManager;
		VulkanPhysicalRenderPassManager m_PhysicalRenderPassManager;
		VulkanPipelineStateManager m_PipelineStateManager;
		VulkanUniformBufferAllocator m_UniformBufferAllocator;
		VulkanSemaphoreAllocator m_SemaphoreAllocator;

		RenderGraph m_RenderGraph;
		std::mutex m_CommandSubmitLock;
		std::map<uint64_t, std::pair<VkCommandBuffer, const RenderGraph::Node*>> m_CommandSubmissions;
		std::unordered_map<uint64_t, VkSemaphore> m_BufferIdToSemaphoreMap;
	};
}

#endif // KH_GFXAPI_VULKAN