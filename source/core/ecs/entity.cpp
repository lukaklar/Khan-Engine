#include "core/precomp.h"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "core/ecs/component.hpp"

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
		child->SetParent(this);
	}

	void Entity::RemoveChild(Entity* child)
	{
		m_Children.erase(child);
		if (child->GetParent() == this)
		{
			child->SetParent(nullptr);
		}
	}
}