#include "core/precomp.h"
#include "core/ecs/world.hpp"
#include "core/ecs/entity.hpp"

namespace Khan
{
	World::World(const char* name)
		: m_Name(name)
	{
	}

	World::~World()
	{
		for (auto& it : m_EntityMap)
		{
			delete it.second;
		}
	}

	Entity* World::CreateEntity()
	{
		Entity* entity = new Entity(m_Registry.create(), this);
		m_EntityMap.emplace(entity->m_EntityHandle, entity);
		m_Entities.push_back(entity);
		return entity;
	}

	void World::DestroyEntity(Entity* entity)
	{
		m_Registry.destroy(entity->m_EntityHandle);
		m_EntityMap.erase(entity->m_EntityHandle);
		m_Entities.erase(std::find(m_Entities.begin(), m_Entities.end(), entity));
		delete entity;
	}
}