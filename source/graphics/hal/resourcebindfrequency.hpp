#pragma once
#include <cstdint>

namespace Khan
{
	enum ResourceBindFrequency : uint32_t
	{
		ResourceBindFrequency_PerFrame,
		ResourceBindFrequency_PerMaterial,
		ResourceBindFrequency_PerDraw,
		ResourceBindFrequency_Count
	};
}