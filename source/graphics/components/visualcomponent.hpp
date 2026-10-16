#pragma once
#include "core/ecs/component.hpp"
#include "core/containers.h"

namespace Khan
{
	struct Mesh;

	struct VisualComponent : Component
	{
		//float m_Scale; // x, y, z
		TArray<Mesh*> m_Meshes;
	};
}