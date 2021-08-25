#pragma once
#include "core/ecs/component.hpp"
#include "graphics/objects/mesh.hpp"
#include <vector>

namespace Khan
{
	struct VisualComponent : Component
	{
		//float m_Scale; // x, y, z
		std::vector<Mesh> m_Meshes;
	};
}