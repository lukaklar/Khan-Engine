#pragma once
#include "core/ecs/world.hpp"

namespace Khan
{
	class System
	{
	public:
		virtual void Update(float dt) = 0;

	protected:
		System(World& world)
			: m_World(world)
		{
		}

		World& m_World;
	};
}