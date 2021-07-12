#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanbarrierrecorder.hpp"
#include "graphics/hal/vulkan/vulkancommandpool.hpp"
#include "graphics/hal/vulkan/vulkandescriptorpool.hpp"
#include "graphics/hal/vulkan/vulkandescriptorsetupdater.hpp"

namespace Khan
{
	enum QueueType;
	enum CommandType;
	class RenderDevice;
	class Buffer;
	class BufferView;
	class VulkanBuffer;
	class VulkanBufferView;
	class ConstantBuffer;
	class PhysicalRenderPass;
	class RenderPass;
	class Texture;
	class TextureView;
	class VulkanTexture;
	class VulkanTextureView;
	struct RenderPipelineState;
	struct VulkanPipelineState;

	class RenderContext
	{
	public:
		RenderContext(RenderDevice& device);
		~RenderContext();

		inline RenderDevice& GetDevice() const { return m_Device; }

		void BeginRecording(const RenderPass& pass);
		void EndRecording();

		void BeginPhysicalRenderPass(const PhysicalRenderPass& renderPass, TextureView* renderTargets[], TextureView* depthStencilBuffer);
		void EndPhysicalRenderPass();

		void SetVertexBuffer(uint32_t location, Buffer* vertexBuffer, uint32_t offset);
		void SetIndexBuffer(Buffer* indexBuffer, uint32_t offset, bool useTwoByteIndex);

		void SetConstantBuffer(ResourceBindFrequency frequency, uint32_t binding, ConstantBuffer* cbuffer);
		void SetSRVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture);
		void SetSRVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer);
		void SetUAVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture);
		void SetUAVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer);
		// TODO: void SetSampler();

		void SetPipelineState(const RenderPipelineState& pipelineState);

		void SetViewport(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
		void SetScissor(int32_t left, int32_t top, uint32_t width, uint32_t height);

		void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
		void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
		// TODO: DrawIndirect
		void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ);
		// TODO: DispatchIndirect

		void ResetFrame(uint32_t frameIndex);

	private:
		void BindPipeline();
		void InsertBarriers();
		void BindDynamicStates();
		void BindResources();

		RenderDevice& m_Device;

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