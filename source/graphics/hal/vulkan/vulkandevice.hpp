#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/renderdevice.hpp"
#include "graphics/hal/vulkan/vulkandevicememorymanager.hpp"
#include "graphics/hal/vulkan/vulkanphysicalrenderpassmanager.hpp"
#include "graphics/hal/vulkan/vulkanpipelinestatemanager.hpp"
#include "graphics/hal/vulkan/vulkansemaphoreallocator.hpp"
#include "graphics/hal/vulkan/vulkantransientresourcemanager.hpp"
#include "graphics/hal/vulkan/vulkanuniformbufferallocator.hpp"
#include "graphics/hal/vulkan/vulkanuploadmanager.hpp"
#include "graphics/hal/vulkan/vulkancommandpool.hpp"
#include "graphics/hal/vulkan/vulkanbarrierrecorder.hpp"

namespace Khan
{
	struct BufferViewDesc;
	struct ShaderDesc;
	struct TextureViewDesc;
	class BufferView;
	class DisplayAdapter;
	class Shader;
	class TextureView;

	class VulkanRenderDevice : public RenderDevice
	{
		friend class VulkanRenderContext;
		friend class VulkanSwapchain;
	public:
		VulkanRenderDevice(const DisplayAdapter& adapter);
		virtual ~VulkanRenderDevice() override;

		inline VkDevice VulkanDevice() const { return m_Device; }

		virtual Buffer* CreateBuffer(const BufferDesc& desc, const void* initData = nullptr) override;
		virtual BufferView* CreateBufferView(Buffer* buffer, const BufferViewDesc& desc) override;

		virtual Texture* CreateTexture(const TextureDesc& desc, const void* initData = nullptr) override;
		virtual TextureView* CreateTextureView(Texture* texture, const TextureViewDesc& desc) override;

		virtual Shader* CreateShader(const ShaderDesc& desc) override;

		virtual PhysicalRenderPass* CreatePhysicalRenderPass(const PhysicalRenderPassDescription& desc) override;

		virtual RenderPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineDescription& desc) override;
		virtual RenderPipelineState* CreateComputePipelineState(const ComputePipelineDescription& desc) override;

		virtual void DestroyBuffer(Buffer* buffer) override;
		virtual void DestroyBufferView(BufferView* view) override;

		virtual void DestroyTexture(Texture* texture) override;
		virtual void DestroyTextureView(TextureView* view) override;

		virtual void DestroyShader(Shader* shader) override;

		virtual void WaitIdle() override;

		virtual void OnFlip(uint32_t frameIndex) override;

	private:
		void SubmitCommands(VkCommandBuffer commandBuffer, const RenderGraph::Node* rgSubmitInfo);
		void FlushCommands();

		VkDevice m_Device;
		VkQueue m_CommandQueues[QueueType_Count];

		VulkanDeviceMemoryManager m_MemoryManager;
		VulkanTransientResourceManager m_TransientResourceManager;
		VulkanPhysicalRenderPassManager m_PhysicalRenderPassManager;
		VulkanPipelineStateManager m_PipelineStateManager;
		VulkanUniformBufferAllocator m_UniformBufferAllocator;
		VulkanUploadManager m_UploadManager;
		VulkanSemaphoreAllocator m_SemaphoreAllocator;

		std::mutex m_CommandSubmitLock;
		std::map<uint64_t, std::pair<VkCommandBuffer, const RenderGraph::Node*>> m_CommandSubmissions;
		std::unordered_map<uint64_t, VkSemaphore> m_BufferIdToSemaphoreMap;

		VulkanCommandPool m_GraphicsCommandPool;
		VulkanCommandPool m_CopyCommandPool;
		VulkanBarrierRecorder m_BarrierRecorder;
	};
}

#endif // KH_GFXAPI_VULKAN