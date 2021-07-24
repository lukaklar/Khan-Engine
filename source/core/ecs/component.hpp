#pragma once

namespace Khan
{
	class Entity;

	struct Component
	{
		virtual ~Component() {};

		// Serialize and Deserialize
		//virtual void OnImGuiRender() const = 0;

		Entity* m_Owner = nullptr;
	};
}