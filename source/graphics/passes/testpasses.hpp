#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class TextureView;

	class TestPass : public RenderPass
	{
	public:
		TestPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_FinalOutput;
		TextureView* m_DepthBuffer;
		PhysicalRenderPass* m_PhysicalRenderPass;
		GraphicsPipelineDescription m_PipelineDesc;
		ConstantBuffer m_PerFrameConsts;
	};
}