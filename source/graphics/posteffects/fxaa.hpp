#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	struct RenderPipelineState;
	class TextureView;

	class FXAAPass : public RenderPass
	{
	public:
		FXAAPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_InputTexture;
		TextureView* m_OutputTexture;
		RenderPipelineState* m_PipelineState;
		ConstantBuffer m_ScreenDimensions;
	};
}