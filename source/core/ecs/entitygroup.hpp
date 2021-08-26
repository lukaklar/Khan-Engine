#pragma once
#include <vector>

namespace Khan
{
	class Entity;

	class EntityGroup
	{
	public:
		EntityGroup(std::vector<Entity*>&& entities)
			: m_Entities(entities)
		{
		}

		inline const std::vector<Entity*>& GetEntities() const { return m_Entities; }

	private:
		std::vector<Entity*> m_Entities;
	};
}