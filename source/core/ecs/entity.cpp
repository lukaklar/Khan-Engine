#include "core/precomp.h"
#include "core/ecs/entity.h"
#include "core/ecs/world.h"
#include "core/ecs/component.h"

namespace Khan
{
	Entity::Entity(entt::entity handle, World* world)
		: m_EntityHandle(handle)
		, m_World(world)
	{
	}

	void Entity::AddChild(Entity* child)
	{
		m_Children.insert(child);
	}

	void Entity::RemoveChild(Entity* child)
	{
		m_Children.erase(child);
		if (child->m_ParentEntity == this)
		{
			child->m_ParentEntity = nullptr;
		}
	}
}