#pragma once

namespace Khan
{
	class Entity;

	struct Component
	{
		virtual ~Component() {};

		Entity* m_Owner = nullptr;
	};
}