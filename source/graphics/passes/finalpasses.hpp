#pragma once
#include "graphics/renderpass.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class TextureView;
	struct RenderPipelineState;

	class FinalPass : public RenderPass
	{
	public:
		FinalPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_FinalOutput;
	};
}