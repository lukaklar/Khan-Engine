#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	class BufferView;
	class TextureView;
	struct RenderPipelineState;

	class LuminanceAdaptationPass : public RenderPass
	{
	public:
		LuminanceAdaptationPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_Histogram;
		BufferView* m_Luminance;
		TextureView* m_HDRTexture;
		RenderPipelineState* m_ComputeHistogramPipelineState;
		RenderPipelineState* m_AverageHistogramPipelineState;
		ConstantBuffer m_LuminanceHistogramBuffer;
		ConstantBuffer m_LuminanceHistogramAverageBuffer;
		std::chrono::steady_clock::time_point m_LastFrameTime;

		inline static constexpr uint32_t K_NUM_HISTOGRAM_BINS = 256;
		inline static constexpr float K_MIN_LOG_LUMINANCE = -10.0f;
		inline static constexpr float K_MAX_LOG_LUMINANCE = 2.0f;
		inline static constexpr float K_TAU = 1.1f;
	};

	class TonemappingPass : public RenderPass
	{
	public:
		TonemappingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_AdaptedLuminance;
		TextureView* m_LightAccumulationBuffer;
		TextureView* m_HDROutput;
		RenderPipelineState* m_PipelineState;
	};
}