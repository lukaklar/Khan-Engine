#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"

namespace Khan
{
	class TextureView;
	class PhysicalRenderPass;

	class TransparentPass : public RenderPass
	{
	public:
		TransparentPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_ColorBuffer;
		TextureView* m_DepthBuffer;

		PhysicalRenderPass* m_PhysicalRenderPass;

		GraphicsPipelineDescription m_PipelineDesc;
	};
}