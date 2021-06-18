#pragma once

#include "graphics/hal/blendstate.hpp"
#include "graphics/hal/depthstencilstate.hpp"
#include "graphics/hal/rasterizerstate.hpp"
#include "graphics/hal/vertexinputstate.hpp"

namespace Khan
{
	class PhysicalRenderPass;
	class Shader;

	struct RenderPipelineState
	{
		virtual ~RenderPipelineState() = 0;
	};

	enum class PrimitiveTopology
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		LineListAdjacency,
		LineStripAdjaceny,
		TriangleListAdjacency,
		TriangleStripAdjacency,
		PatchList1CP,
		PatchList2CP,
		PatchList3CP,
		PatchList4CP
	};

	enum class IndexStripCut
	{
		Disabled,
		Value_0xFFFF,
		Value_0xFFFFFFFF
	};

	struct GraphicsPipelineDescription
	{
		const Shader* m_VertexShader = nullptr;
		const Shader* m_PixelShader = nullptr;
		const Shader* m_HullShader = nullptr;
		const Shader* m_DomainShader = nullptr;
		const Shader* m_GeometryShader = nullptr;
		VertexInputState m_VertexInputState;
		DepthStencilState m_DepthStencilState;
		RasterizerState m_RasterizerState;
		BlendState m_BlendState;
		PrimitiveTopology m_PrimitiveTopology = PrimitiveTopology::TriangleList;
		IndexStripCut m_IndexStripCutValue = IndexStripCut::Disabled;
		float m_BlendFactor[4] = { 0.0f };
		const PhysicalRenderPass* m_PhysicalRenderPass = nullptr;

		bool operator==(const GraphicsPipelineDescription& other) const
		{
			return !std::memcmp(this, &other, sizeof(GraphicsPipelineDescription));
		}
	};

	struct ComputePipelineDescription
	{
		const Shader* m_ComputeShader = nullptr;

		bool operator==(const ComputePipelineDescription& other) const
		{
			return m_ComputeShader == other.m_ComputeShader;
		}
	};
}

namespace std
{
	template <>
	struct hash<Khan::GraphicsPipelineDescription>
	{
		std::size_t operator()(const Khan::GraphicsPipelineDescription& key) const;
	};

	template <>
	struct hash<Khan::ComputePipelineDescription>
	{
		std::size_t operator()(const Khan::ComputePipelineDescription& key) const;
	};
}