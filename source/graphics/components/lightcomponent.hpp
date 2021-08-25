#pragma once
#include "core/ecs/component.hpp"

namespace Khan
{
	class Light;

	struct LightComponent : Component
	{
		Light* m_Light = nullptr;
	};
}