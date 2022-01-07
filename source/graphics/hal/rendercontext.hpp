#pragma once
#include <cstdint>
#include <thirdparty/glm/glm.hpp>

namespace Khan
{
	enum ResourceBindFrequency : uint32_t;
	struct RenderPipelineState;
	class RenderDevice;
	class Buffer;
	class BufferView;
	class ConstantBuffer;
	class PhysicalRenderPass;
	class RenderPass;
	class Texture;
	class TextureView;

	class RenderContext
	{
	public:
		virtual ~RenderContext() {}

		virtual RenderDevice& GetDevice() const = 0;

		virtual void BeginRecording(const RenderPass& pass) = 0;
		virtual void EndRecording() = 0;

		virtual void BeginPhysicalRenderPass(const PhysicalRenderPass& renderPass, TextureView* renderTargets[], TextureView* depthStencilBuffer) = 0;
		virtual void EndPhysicalRenderPass() = 0;

		virtual void SetVertexBuffer(uint32_t location, Buffer* vertexBuffer, uint32_t offset) = 0;
		virtual void SetIndexBuffer(Buffer* indexBuffer, uint32_t offset, bool useTwoByteIndex) = 0;

		virtual void SetConstantBuffer(ResourceBindFrequency frequency, uint32_t binding, ConstantBuffer* cbuffer) = 0;
		virtual void SetSRVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture) = 0;
		virtual void SetSRVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer) = 0;
		virtual void SetUAVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture) = 0;
		virtual void SetUAVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer) = 0;
		// TODO: void SetSampler();

		virtual void SetPipelineState(const RenderPipelineState& pipelineState) = 0;

		virtual void SetViewport(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) = 0;
		virtual void SetScissor(int32_t left, int32_t top, uint32_t width, uint32_t height) = 0;

		virtual void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
		virtual void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
		// TODO: DrawIndirect
		virtual void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ) = 0;
		virtual void DispatchIndirect(BufferView* buffer, uint32_t offset) = 0;

		virtual void CopyTexture(TextureView* src, const glm::ivec3& srcOffset, TextureView* dst, const glm::ivec3& dstOffset, const glm::uvec3& size) = 0;

		virtual void UpdateBufferFromHost(BufferView* dst, const void* src) = 0;

		virtual void ResetFrame(uint32_t frameIndex) = 0;
	};
}