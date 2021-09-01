#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	struct RenderPipelineState;
	class BufferView;
	class TextureView;

	class LightDataUploadPass : public RenderPass
	{
	public:
		LightDataUploadPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_LightData;
	};

	class TileFrustumCalculationPass : public RenderPass
	{
	public:
		TileFrustumCalculationPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_PerTileFrustums;

		RenderPipelineState* m_PipelineState;
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

		RenderPipelineState* m_PipelineState;

		ConstantBuffer m_LightParams;
	};

	class TiledDeferredLightingPass : public RenderPass
	{
	public:
		TiledDeferredLightingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_LightIndexList;
		BufferView* m_LightData;
		TextureView* m_LightGrid;

		TextureView* m_GBuffer_Albedo;
		TextureView* m_GBuffer_Normals;
		TextureView* m_GBuffer_Emissive;
		TextureView* m_GBuffer_SpecularReflectance;
		TextureView* m_GBuffer_MetallicAndRoughness;
		//TextureView* m_GBuffer_MotionVectors;
		TextureView* m_GBuffer_Depth;

		TextureView* m_LightingResult;

		RenderPipelineState* m_PipelineState;
	};
}