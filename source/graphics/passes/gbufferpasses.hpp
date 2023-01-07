#pragma once
#include "graphics/renderpass.hpp"
#include "graphicshal/constantbuffer.hpp"
#include "graphicshal/pipelinedescriptions.hpp"

namespace Khan
{
	class TextureView;
	class PhysicalRenderPass;

	class GBufferPass : public RenderPass
	{
	public:
		GBufferPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_GBuffer_Albedo;
		TextureView* m_GBuffer_Normals;
		TextureView* m_GBuffer_Emissive;
		TextureView* m_GBuffer_PBRConsts;
		TextureView* m_GBuffer_Depth;

		GraphicsPipelineDescription m_PipelineDesc;

		ConstantBuffer m_PerFrameConsts;
	};
}