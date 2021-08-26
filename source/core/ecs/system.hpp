#pragma once

namespace Khan
{
	class System
	{
	public:
		virtual ~System() {}

		virtual void Update(float dt) const = 0;
	};
}