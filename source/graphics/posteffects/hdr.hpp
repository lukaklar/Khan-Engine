#pragma once
#include "graphics/renderpass.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class TextureView;
	struct RenderPipelineState;

	class HDRPass : public RenderPass
	{
	public:
		HDRPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_LightAccumulationBuffer;
		TextureView* m_FinalOutput;
		RenderPipelineState* m_PipelineState;
	};
}