#pragma once
#include "core/ecs/system.hpp"

namespace Khan
{
	class MeshCullingSystem : public System
	{
	public:
		virtual void Update(float dt) const override;
	};
}