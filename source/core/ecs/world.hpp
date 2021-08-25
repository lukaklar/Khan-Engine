#pragma once
#include "core/ecs/entitygroup.hpp"
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

		inline const std::vector<Entity*>& GetEntities() const { return m_Entities; }

		template<typename... ComponentTypes>
		std::shared_ptr<EntityGroup> GetEntityGroup()
		{
			std::vector<Entity*> entities;

			auto view = m_Registry.view<ComponentTypes...>();
			for (entt::entity id : view)
			{
				entities.push_back(m_EntityMap.find(id)->second);
			}

			return std::make_shared<EntityGroup>(std::move(entities));
		}

		inline const std::string& GetName() const { return m_Name; }

	private:
		static World* ms_CurrentWorld;

		entt::registry m_Registry;
		// Possibility to optimize: create default constructor for Entity and create a fixed size array where you would create Entities
		std::map<entt::entity, Entity*> m_EntityMap;
		std::vector<Entity*> m_Entities;

		std::string m_Name;
	};
}
