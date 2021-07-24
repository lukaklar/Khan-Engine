#pragma once

namespace Khan
{
	class Entity;

	template<typename... ComponentTypes>
	class EntityGroup
	{
	public:
		EntityGroup(std::vector<Entity*>&& entities)
			: m_Entities(entities)
		{
		}

		inline const std::vector<Entity*>& GetEntites() const { return m_Entities; }

	private:
		std::vector<Entity*> m_Entities;
	};
}