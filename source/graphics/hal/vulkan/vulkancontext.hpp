#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/rendercontext.hpp"
#include "graphics/hal/vulkan/vulkanbarrierrecorder.hpp"
#include "graphics/hal/vulkan/vulkancommandpool.hpp"
#include "graphics/hal/vulkan/vulkandescriptorpool.hpp"
#include "graphics/hal/vulkan/vulkandescriptorsetupdater.hpp"

namespace Khan
{
	enum QueueType;
	enum CommandType;
	struct VulkanPipelineState;
	class VulkanBufferView;
	class VulkanRenderDevice;
	class VulkanTexture;
	class VulkanTextureView;

	class VulkanRenderContext : public RenderContext
	{
	public:
		VulkanRenderContext(RenderDevice& device);
		virtual ~VulkanRenderContext();

		virtual RenderDevice& GetDevice() const override;

		virtual void BeginRecording(const RenderPass& pass) override;
		virtual void EndRecording() override;

		virtual void BeginPhysicalRenderPass(const PhysicalRenderPass& renderPass, TextureView* renderTargets[], TextureView* depthStencilBuffer) override;
		virtual void EndPhysicalRenderPass() override;

		virtual void SetVertexBuffer(uint32_t location, Buffer* vertexBuffer, uint32_t offset) override;
		virtual void SetIndexBuffer(Buffer* indexBuffer, uint32_t offset, bool useTwoByteIndex) override;

		virtual void SetConstantBuffer(ResourceBindFrequency frequency, uint32_t binding, ConstantBuffer* cbuffer) override;
		virtual void SetSRVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture) override;
		virtual void SetSRVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer) override;
		virtual void SetUAVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture) override;
		virtual void SetUAVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer) override;
		// TODO: void SetSampler();

		virtual void SetPipelineState(const RenderPipelineState& pipelineState) override;

		virtual void SetViewport(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) override;
		virtual void SetScissor(int32_t left, int32_t top, uint32_t width, uint32_t height) override;

		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
		// TODO: DrawIndirect
		virtual void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ) override;
		// TODO: DispatchIndirect

		virtual void ResetFrame(uint32_t frameIndex) override;

	private:
		void BindPipeline();
		void UploadResources();
		void InsertBarriers();
		void BindDynamicStates();
		void BindResources();

		void UploadBufferFromHost(VulkanBuffer& dst);
		void UploadImageFromHost(VulkanTexture& dst);

		VulkanRenderDevice& m_Device;

		VkCommandBuffer m_CommandBuffer;
		const RenderPass* m_ExecutingPass;
		CommandType m_CommandType;
		VulkanCommandPool m_CommandPools[2];
		VulkanBarrierRecorder m_BarrierRecorder;

		std::vector<VkFramebuffer> m_Framebuffers[K_MAX_FRAMES_IN_FLIGHT];
		std::vector<VkFramebuffer>* m_CurrentFramebuffers;

		VulkanBuffer* m_VertexBuffers[K_MAX_VERTEX_DATA_STREAMS];
		VkDeviceSize m_VBOffsets[K_MAX_VERTEX_DATA_STREAMS];
		std::bitset<K_MAX_VERTEX_DATA_STREAMS> m_VBDirty;

		VulkanBuffer* m_IndexBuffer;
		VkDeviceSize m_IBOffset;
		VkIndexType m_IndexType;
		bool m_ShouldBindIndexBuffer;
		bool m_IndexBufferDirty;

		VulkanDescriptorPool m_DescriptorPool;
		VulkanDescriptorSetUpdater m_DescriptorUpdater;
		uint32_t m_FirstDirtySet;

		ConstantBuffer* m_CBuffers[ResourceBindFrequency_Count][K_MAX_CBV];
		VulkanTextureView* m_SRVTextures[ResourceBindFrequency_Count][K_MAX_SRV];
		VulkanBufferView* m_SRVBuffers[ResourceBindFrequency_Count][K_MAX_SRV];
		VulkanTextureView* m_UAVTextures[ResourceBindFrequency_Count][K_MAX_UAV];
		VulkanBufferView* m_UAVBuffers[ResourceBindFrequency_Count][K_MAX_UAV];

		const VulkanPipelineState* m_PipelineState;
		bool m_PipelineDirty;

		VkViewport m_Viewport;
		VkRect2D m_Scissor;
		bool m_ViewportDirty;
		bool m_ScissorDirty;

#ifdef KH_DEBUG
		bool m_IsRenderPassInProgress;
#endif // KH_DEBUG
	};
}

#endif // KH_GFXAPI_VULKAN