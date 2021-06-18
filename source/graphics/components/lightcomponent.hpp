#pragma once
#include "core/ecs/component.h"

class Light;

namespace Khan
{
	struct LightComponent : Component
	{
		Light* m_Light = nullptr;
	};
}