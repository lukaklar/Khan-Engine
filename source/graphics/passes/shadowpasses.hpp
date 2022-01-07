#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class TextureView;
	struct RenderPipelineState;

	class ShadowPass : public RenderPass
	{
	public:
		ShadowPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_ShadowMap;
		PhysicalRenderPass* m_PhysicalRenderPass;
		RenderPipelineState* m_PipelineStateNoCulling;
		RenderPipelineState* m_PipelineStateBackfaceCulling;
		ConstantBuffer m_ViewProjParams;
	};
}