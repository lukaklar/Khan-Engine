#pragma once
#include "graphics/renderpass.hpp"
#include "graphics/hal/constantbuffer.hpp"

namespace Khan
{
	struct RenderPipelineState;
	class PhysicalRenderPass;
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

	class ClusterCalculationPass : public RenderPass
	{
	public:
		ClusterCalculationPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_Clusters;
		RenderPipelineState* m_PipelineState;
	};

	class MarkActiveClustersPass : public RenderPass
	{
	public:
		MarkActiveClustersPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		TextureView* m_DepthTexture;
		BufferView* m_ActiveClusterFlags;
		PhysicalRenderPass* m_PhysicalRenderPass;
		RenderPipelineState* m_PipelineState;
		ConstantBuffer m_PerFrameConsts;
	};

	class CompactActiveClustersPass : public RenderPass
	{
	public:
		CompactActiveClustersPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_ActiveClusterFlags;
		BufferView* m_ActiveClusterIndexList;
		BufferView* m_CullingDispatchArgs;
		RenderPipelineState* m_PipelineState;
		ConstantBuffer m_ClusterParams;
	};

	class LightCullingPass : public RenderPass
	{
	public:
		LightCullingPass();

		virtual void Setup(RenderGraph& renderGraph, Renderer& renderer) override;
		virtual void Execute(RenderContext& context, Renderer& renderer) override;

	private:
		BufferView* m_Clusters;
		BufferView* m_ActiveClusterIndexList;
		BufferView* m_Lights;
		BufferView* m_LightIndexCounter;
		BufferView* m_LightIndexList;
		BufferView* m_LightGrid;
		BufferView* m_IndirectDispatchArgs;
		RenderPipelineState* m_PipelineState;
		ConstantBuffer m_LightParams;
	};
}