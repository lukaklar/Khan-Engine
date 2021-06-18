#include "core/precomp.h"
#include "core/ecs/world.h"
#include "core/ecs/entity.h"

namespace Khan
{
	World::World(const std::string& name)
		: m_Name(name)
	{
		// TODO: Load the world
	}

	World::~World()
	{
		for (auto& it : m_Entities)
		{
			delete it.second;
		}
	}

	Entity* World::CreateEntity()
	{
		Entity* entity = new Entity(m_Registry.create(), this);
		m_Entities.emplace(entity->m_EntityHandle, entity);
		return entity;
	}

	void World::DestroyEntity(Entity* entity)
	{
		m_Registry.destroy(entity->m_EntityHandle);
		m_Entities.erase(entity->m_EntityHandle);
		delete entity;
	}
}