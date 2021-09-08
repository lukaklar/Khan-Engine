#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	class BufferView;
	class TextureView;
	struct RenderPipelineState;

	/*class HDRPass : public RenderPass
	{
	public:
		HDRPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		ConstantBuffer m_DownScaleConstants;
		ConstantBuffer m_TonemapConstants;

		BufferView* m_IntermediateLuminanceValues;
		BufferView* m_AverageLuminanceValue;

		TextureView* m_LightAccumulationBuffer;
		TextureView* m_HDROutput;
		RenderPipelineState* m_DownScalePass1PipelineState;
		RenderPipelineState* m_DownScalePass2PipelineState;
		RenderPipelineState* m_TonemapPassPipelineState;
	};*/

	class HDRPass : public RenderPass
	{
	public:
		HDRPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_LightAccumulationBuffer;
		TextureView* m_HDROutput;
		RenderPipelineState* m_PipelineState;
	};
}