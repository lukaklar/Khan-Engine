#pragma once
#include "core/ecs/component.hpp"
#include "core/camera/camera.hpp"

namespace Khan
{
	struct CameraComponent : Component
	{
		Camera* m_Camera;
	};
}