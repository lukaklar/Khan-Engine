#pragma once
#include "core/primitivetypes.h"

namespace Khan
{
	enum ResourceBindFrequency : uint32
	{
		ResourceBindFrequency_PerFrame,
		ResourceBindFrequency_PerMaterial,
		ResourceBindFrequency_PerDraw,
		ResourceBindFrequency_Count
	};
}