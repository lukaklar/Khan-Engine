#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"
#include "graphics/hal/pipelinedescriptions.hpp"

namespace Khan
{
	class BufferView;
	class TextureView;

	class TileFrustumCalculationPass : public RenderPass
	{
	public:
		TileFrustumCalculationPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_PerTileFrustums;

		RenderPipelineState* m_PipelineState;

		// TODO: Move these up to renderer so they can be shared between passes
		ConstantBuffer m_DispatchParams;
		ConstantBuffer m_ScreenToViewParams;
	};

	class LightCullingPass : public RenderPass
	{
	public:
		LightCullingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_PerTileFrustums;
		BufferView* m_Lights;

		TextureView* m_DepthTexture;

		BufferView* m_OpaqueLightIndexCounter;
		BufferView* m_TransparentLightIndexCounter;

		BufferView* m_OpaqueLightIndexList;
		BufferView* m_TransparentLightIndexList;
		TextureView* m_OpaqueLightGrid;
		TextureView* m_TransparentLightGrid;

		// TODO: Move these up to renderer so they can be shared between passes
		ConstantBuffer m_DispatchParams;
		ConstantBuffer m_ScreenToViewParams;
	};

	class TiledDeferredLightingPass : public RenderPass
	{
	public:
		TiledDeferredLightingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_GBuffer_Albedo;
		TextureView* m_GBuffer_Normals;
		TextureView* m_GBuffer_Emissive;
		TextureView* m_GBuffer_SpecularReflectance;
		TextureView* m_GBuffer_MetallicAndRoughness;
		TextureView* m_GBuffer_MotionVectors;
		TextureView* m_GBuffer_Depth;

		TextureView* m_LightAccumulationBuffer;

		RenderPipelineState* m_PipelineState;

		// TODO: Move these up to renderer so they can be shared between passes
		ConstantBuffer m_DispatchParams;
		ConstantBuffer m_GBufferUnpackParams;
	};
}