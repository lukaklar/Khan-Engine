#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	struct RenderPipelineState;
	class BufferView;
	class TextureView;

	class ClusterDeferredLightingPass : public RenderPass
	{
	public:
		ClusterDeferredLightingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_LightData;
		BufferView* m_LightIndexList;
		BufferView* m_LightGrid;

		TextureView* m_GBuffer_Albedo;
		TextureView* m_GBuffer_Normals;
		TextureView* m_GBuffer_Emissive;
		TextureView* m_GBuffer_PBRConsts;
		TextureView* m_GBuffer_Depth;
		TextureView* m_AOTexture;

		TextureView* m_LightingResult;

		RenderPipelineState* m_PipelineState;
	};

	class DeferredLightingPass : public RenderPass
	{
	public:
		DeferredLightingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_LightData;

		TextureView* m_GBuffer_Albedo;
		TextureView* m_GBuffer_Normals;
		TextureView* m_GBuffer_Emissive;
		TextureView* m_GBuffer_PBRConsts;
		TextureView* m_GBuffer_Depth;
		TextureView* m_AOTexture;
		TextureView* m_LightingResult;

		ConstantBuffer m_LightParams;

		RenderPipelineState* m_PipelineState;
	};
}