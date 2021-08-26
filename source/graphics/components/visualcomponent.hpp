#pragma once
#include "core/ecs/component.hpp"
#include <vector>

namespace Khan
{
	struct Mesh;

	struct VisualComponent : Component
	{
		//float m_Scale; // x, y, z
		std::vector<Mesh*> m_Meshes;
	};
}