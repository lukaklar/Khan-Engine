#pragma once
#include "graphics/renderpass.hpp"
#include "graphicshal/constantbuffer.hpp"

namespace Khan
{
	struct RenderPipelineState;
	class TextureView;

	class SSAOPass : public RenderPass
	{
	public:
		SSAOPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_GBuffer_Normals;
		TextureView* m_GBuffer_Depth;
		TextureView* m_SSAOTexture;
		TextureView* m_SSAOBlurTexture;
		RenderPipelineState* m_CalculationPipelineState;
		RenderPipelineState* m_BlurPipelineState;
		ConstantBuffer m_SampleParams;
		ConstantBuffer m_ProjectionParams;
	};
}