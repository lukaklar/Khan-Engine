#pragma once
#include "graphics/rendergraph.hpp"
#include <cstdint>
#include <vector>

namespace Khan
{
	struct ComputePipelineDescription;
	struct GraphicsPipelineDescription;
	struct PhysicalRenderPassDescription;
	struct RenderPipelineState;
	struct ShaderDesc;
	class DisplayAdapter;
	class PhysicalRenderPass;
	class RenderContext;
	class Shader;

	class RenderDevice
	{
	public:
		RenderDevice(const DisplayAdapter& adapter, TransientResourceManager* manager)
			: m_Adapter(adapter)
			, m_FrameCounter(0)
			, m_RenderGraph(manager)
		{
		}

		virtual ~RenderDevice() {}

		inline const DisplayAdapter& GetAdapter() const { return m_Adapter; }
		inline const std::vector<RenderContext*>& GetContexts() const { return m_Contexts; }
		inline RenderGraph& GetRenderGraph() { return m_RenderGraph; }
		inline uint64_t FrameNumber() const { return m_FrameCounter; }

		virtual Buffer* CreateBuffer(const BufferDesc& desc, const void* initData = nullptr) = 0;
		virtual BufferView* CreateBufferView(Buffer* buffer, const BufferViewDesc& desc) = 0;

		virtual Texture* CreateTexture(const TextureDesc& desc, const void* initData = nullptr) = 0;
		virtual TextureView* CreateTextureView(Texture* texture, const TextureViewDesc& desc) = 0;

		virtual Shader* CreateShader(const ShaderDesc& desc) = 0;

		virtual PhysicalRenderPass* CreatePhysicalRenderPass(const PhysicalRenderPassDescription& desc) = 0;

		virtual RenderPipelineState* CreateGraphicsPipelineState(const GraphicsPipelineDescription& desc) = 0;
		virtual RenderPipelineState* CreateComputePipelineState(const ComputePipelineDescription& desc) = 0;

		virtual void DestroyBuffer(Buffer* buffer) = 0;
		virtual void DestroyBufferView(BufferView* view) = 0;

		virtual void DestroyTexture(Texture* texture) = 0;
		virtual void DestroyTextureView(TextureView* view) = 0;

		virtual void DestroyShader(Shader* shader) = 0;

		virtual void WaitIdle() = 0;

		virtual void OnFlip(uint32_t frameIndex) = 0;

	protected:
		std::vector<RenderContext*> m_Contexts;
		const DisplayAdapter& m_Adapter;
		uint64_t m_FrameCounter;

		RenderGraph m_RenderGraph;
	};
}