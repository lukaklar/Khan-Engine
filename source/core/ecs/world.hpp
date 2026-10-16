#pragma once
#include "core/ecs/entitygroup.hpp"
#include "core/memory.h"
#include "core/string.h"
#include "core/utility.h"
#include <thirdparty/entt/entt.hpp>

namespace Khan
{
	class World
	{
		friend class Entity;
	public:
		static World* GetCurrentWorld();
		static void SetCurrentWorld(World* world);

		World(const char* name);
		~World();

		Entity* CreateEntity();
		void DestroyEntity(Entity* entity);

		inline const TArray<Entity*>& GetEntities() const { return m_Entities; }

		template<typename... ComponentTypes>
		TSharedPtr<EntityGroup> GetEntityGroup()
		{
			TArray<Entity*> entities;

			auto view = m_Registry.view<ComponentTypes...>();
			for (entt::entity id : view)
			{
				entities.push_back(m_EntityMap.find(id)->second);
			}

			return MakeShared<EntityGroup>(Move(entities));
		}

		inline const String& GetName() const { return m_Name; }

	private:
		static World* ms_CurrentWorld;

		entt::registry m_Registry;
		// Possibility to optimize: create default constructor for Entity and create a fixed size array where you would create Entities
		TMap<entt::entity, Entity*> m_EntityMap;
		TArray<Entity*> m_Entities;

		String m_Name;
	};
}
