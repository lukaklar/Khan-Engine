#pragma once
#include "core/math.h"
#include "core/primitivetypes.h"

namespace Khan
{
	enum ResourceBindFrequency : uint32;
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

		virtual void SetVertexBuffer(uint32 location, Buffer* vertexBuffer, uint32 offset) = 0;
		virtual void SetIndexBuffer(Buffer* indexBuffer, uint32 offset, bool useTwoByteIndex) = 0;

		virtual void SetConstantBuffer(ResourceBindFrequency frequency, uint32 binding, ConstantBuffer* cbuffer) = 0;
		virtual void SetSRVTexture(ResourceBindFrequency frequency, uint32 binding, TextureView* texture) = 0;
		virtual void SetSRVBuffer(ResourceBindFrequency frequency, uint32 binding, BufferView* buffer) = 0;
		virtual void SetUAVTexture(ResourceBindFrequency frequency, uint32 binding, TextureView* texture) = 0;
		virtual void SetUAVBuffer(ResourceBindFrequency frequency, uint32 binding, BufferView* buffer) = 0;
		// TODO: void SetSampler();

		virtual void SetPipelineState(const RenderPipelineState& pipelineState) = 0;

		virtual void SetViewport(float left, float top, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f) = 0;
		virtual void SetScissor(int32 left, int32 top, uint32 width, uint32 height) = 0;

		virtual void DrawInstanced(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) = 0;
		virtual void DrawIndexedInstanced(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) = 0;
		// TODO: DrawIndirect
		virtual void Dispatch(uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ) = 0;
		virtual void DispatchIndirect(BufferView* buffer, uint32 offset) = 0;

		virtual void ClearBuffer(BufferView* buffer, uint32 clearValue) = 0;
		virtual void CopyTexture(TextureView* src, const Vector3I& srcOffset, TextureView* dst, const Vector3I& dstOffset, const Vector3U& size) = 0;

		virtual void UpdateBufferFromHost(BufferView* dst, const void* src) = 0;

		virtual void ResetFrame(uint32 frameIndex) = 0;
	};
}