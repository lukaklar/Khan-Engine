#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/primitivetypes.h"
#include "graphicshal/rendercontext.hpp"
#include "graphicshal/vulkan/vulkanbarrierrecorder.hpp"
#include "graphicshal/vulkan/vulkancommandpool.hpp"
#include "graphicshal/vulkan/vulkandescriptorpool.hpp"
#include "graphicshal/vulkan/vulkandescriptorsetupdater.hpp"

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

		virtual void SetVertexBuffer(uint32 location, Buffer* vertexBuffer, uint32 offset) override;
		virtual void SetIndexBuffer(Buffer* indexBuffer, uint32 offset, bool useTwoByteIndex) override;

		virtual void SetConstantBuffer(ResourceBindFrequency frequency, uint32 binding, ConstantBuffer* cbuffer) override;
		virtual void SetSRVTexture(ResourceBindFrequency frequency, uint32 binding, TextureView* texture) override;
		virtual void SetSRVBuffer(ResourceBindFrequency frequency, uint32 binding, BufferView* buffer) override;
		virtual void SetUAVTexture(ResourceBindFrequency frequency, uint32 binding, TextureView* texture) override;
		virtual void SetUAVBuffer(ResourceBindFrequency frequency, uint32 binding, BufferView* buffer) override;
		// TODO: void SetSampler();

		virtual void SetPipelineState(const RenderPipelineState& pipelineState) override;

		virtual void SetViewport(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) override;
		virtual void SetScissor(int32 left, int32 top, uint32 width, uint32 height) override;

		virtual void DrawInstanced(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
		virtual void DrawIndexedInstanced(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) override;
		// TODO: DrawIndirect
		virtual void Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) override;
		virtual void DispatchIndirect(BufferView* buffer, uint32 offset) override;

		virtual void ClearBuffer(BufferView* buffer, uint32 clearValue) override;
		virtual void CopyTexture(TextureView* src, const Vector3I& srcOffset, TextureView* dst, const Vector3I& dstOffset, const Vector3U& size) override;

		virtual void UpdateBufferFromHost(BufferView* dst, const void* src) override;

		virtual void ResetFrame(uint32 frameIndex) override;

	private:
		void BindPipeline();
		void InsertBarriers();
		void BindDynamicStates();
		void BindResources();

		VulkanRenderDevice& m_Device;

		VkCommandBuffer m_CommandBuffer;
		const RenderPass* m_ExecutingPass;
		CommandType m_CommandType;
		VulkanCommandPool m_CommandPools[QueueType_Count];
		VulkanBarrierRecorder m_BarrierRecorder;

		TArray<VkFramebuffer> m_Framebuffers[K_MAX_FRAMES_IN_FLIGHT];
		TArray<VkFramebuffer>* m_CurrentFramebuffers;

		VulkanBuffer* m_VertexBuffers[K_MAX_VERTEX_DATA_STREAMS];
		VkDeviceSize m_VBOffsets[K_MAX_VERTEX_DATA_STREAMS];
		BitSet<K_MAX_VERTEX_DATA_STREAMS> m_VBDirty;

		VulkanBuffer* m_IndexBuffer;
		VkDeviceSize m_IBOffset;
		VkIndexType m_IndexType;
		bool m_ShouldBindIndexBuffer;
		bool m_IndexBufferDirty;

		VulkanDescriptorPool m_DescriptorPool;
		VulkanDescriptorSetUpdater m_DescriptorUpdater;
		uint32 m_FirstDirtySet;

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