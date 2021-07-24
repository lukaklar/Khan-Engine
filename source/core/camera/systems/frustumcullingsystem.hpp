#pragma once
#include "core/ecs/system.hpp"

namespace Khan
{
	class FrustumCullingSystem : public System
	{
	public:
		virtual void Update(float dt) override;
	};
}