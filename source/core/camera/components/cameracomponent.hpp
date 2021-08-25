#pragma once
#include "core/ecs/component.hpp"

namespace Khan
{
	class Camera;

	struct CameraComponent : Component
	{
		Camera* m_Camera;
	};
}