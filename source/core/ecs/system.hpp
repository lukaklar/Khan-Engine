#pragma once

namespace Khan
{
	class System
	{
	public:
		virtual ~System() = 0;

		virtual void Update(float dt) = 0;
	};
}