#pragma once
#include <cstdint>

namespace Khan
{
	struct RasterizerState
	{
		enum class CullMode
		{
			None,
			Front,
			Back
		};

		CullMode m_CullMode = CullMode::None;
		bool m_WireframeEnabled = false;
		bool m_FrontCounterClockwise = false;
		bool m_ConservativeRasterizationEnabled = false;
		bool m_DepthBiasEnabled = false;
		uint32_t m_DepthBias = 0;
		float m_DepthBiasClamp = 0.0f;
		float m_SlopeScaledDepthBias = 0.0f;
	};
}