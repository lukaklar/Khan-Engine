#pragma once
#include "graphics/renderpass.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class TextureView;
	struct RenderPipelineState;

	class TestPass : public RenderPass
	{
	public:
		TestPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_FinalOutput;
		PhysicalRenderPass* m_PhysicalRenderPass;
		RenderPipelineState* m_PipelineState;
	};
}