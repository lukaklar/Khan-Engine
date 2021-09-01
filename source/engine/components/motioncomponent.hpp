#pragma once
#include "core/ecs/component.hpp"

namespace Khan
{
	struct MotionComponent : Component
	{
		float m_MovementSpeed;
		float m_RotationSpeed;
	};
}