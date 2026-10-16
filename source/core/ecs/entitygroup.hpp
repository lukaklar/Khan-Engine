#pragma once
#include "core/containers.h"

namespace Khan
{
	class Entity;

	class EntityGroup
	{
	public:
		EntityGroup(TArray<Entity*>&& entities)
			: m_Entities(entities)
		{
		}

		inline const TArray<Entity*>& GetEntities() const { return m_Entities; }

	private:
		TArray<Entity*> m_Entities;
	};
}