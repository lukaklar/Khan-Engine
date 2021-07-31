#pragma once
#include "core/ecs/system.hpp"

namespace Khan
{
	class CameraSystem : public System
	{
	public:
		virtual void Update(float dt) override;
	};
}