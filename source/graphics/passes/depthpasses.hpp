#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class TextureView;
	struct RenderPipelineState;

	class DepthPrePass : public RenderPass
	{
	public:
		DepthPrePass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_DepthBuffer;
		PhysicalRenderPass* m_PhysicalRenderPass;
		RenderPipelineState* m_PipelineStateNoCulling;
		RenderPipelineState* m_PipelineStateBackfaceCulling;
		ConstantBuffer m_ViewProjParams;
	};
}