#pragma once
#include <thirdparty/entt/entt.hpp>

namespace Khan
{
	class World
	{
		friend class Entity;
	public:
		World(const std::string& name);
		~World();

		Entity* CreateEntity();
		void DestroyEntity(Entity* entity);

		inline const std::string& GetName() const { return m_Name; }

	private:
		entt::registry m_Registry;
		// Possibility to optimize: create default constructor for Entity and create a fixed size array where you would create Entities
		std::map<entt::entity, Entity*> m_Entities;

		std::string m_Name;
	};
}
