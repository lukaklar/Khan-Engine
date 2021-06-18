#pragma once
#include "core/ecs/component.h"

namespace Khan
{
	class Renderable;

	struct VisualComponent : Component
	{
		float m_Scale; // x, y, z
		float m_DistanceFromCamera;
		Renderable* m_Renderable;
	};
}