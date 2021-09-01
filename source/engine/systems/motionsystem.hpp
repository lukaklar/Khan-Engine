#pragma once
#include "core/ecs/system.hpp"
#include <thirdparty/glm/glm.hpp>

namespace Khan
{
	class MotionSystem : public System
	{
	public:
		virtual void Update(float dt) const override;
	};
}