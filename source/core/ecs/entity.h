#pragma once
#include "core/boundingvolume.h"
#include <thirdparty/entt/entt.hpp>

namespace Khan
{
	struct Component;

	class Entity
	{
		friend class World;
	public:
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "New components need to inherit from the Component base class.");
			T& component = m_World->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			component.m_Owner = this;
			m_Components.emplace_back(&component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			return m_World->m_Registry.get<T>(m_EntityHandle);
		}

#ifdef KH_DEBUG
		template<typename T>
		bool HasComponent()
		{
			return m_World->m_Registry.all_of<T>(m_EntityHandle);
		}
#endif // KH_DEBUG

		template<typename T>
		void RemoveComponent()
		{
			Component* component = &m_World->m_Registry.get<T>(m_EntityHandle);
			m_World->m_Registry.remove<T>(m_EntityHandle);
			m_Components.erase(std::find(m_Components.begin(), m_Components.end(), component));
		}

		void AddChild(Entity* child);
		void RemoveChild(Entity* child);

		inline bool IsValid() const { return m_EntityHandle != entt::null; }
		inline uint32_t GetID() const { return static_cast<uint32_t>(m_EntityHandle); }
		inline World* GetWorld() const { return m_World; }
		inline Entity* GetParent() const { return m_ParentEntity; }
		inline void SetParent(Entity* value) { m_ParentEntity = value; }
		inline const std::set<Entity*>& GetChildren() const { return m_Children; }
		inline const std::vector<Component*> GetComponents() const { return m_Components; }
		inline const std::string& GetName() const { return m_Name; }
		inline void SetName(const std::string& value) { m_Name = value; }
		inline const BoundingVolume& GetBoundingVolume() const { return m_BoundingVolume; }
		inline void SetBoundingVolume(const BoundingVolume& value) { m_BoundingVolume = value; }
		inline bool IsInFrustum() const { return m_InFrustum; }
		inline void SetInFrustum(bool value) { m_InFrustum = value; }

	private:
		Entity() = default;
		Entity(entt::entity handle, World* world);

		entt::entity m_EntityHandle = entt::null;
		World* m_World = nullptr;
		Entity* m_ParentEntity = nullptr;
		std::set<Entity*> m_Children;
		std::vector<Component*> m_Components;
		std::string m_Name = "Empty Entity";
		glm::vec4 m_GlobalPosition;
		glm::quat m_GlobalRotation;
		glm::mat4 m_GlobalTransform;
		glm::vec4 m_LocalPosition;
		glm::quat m_LocalRotation;
		glm::mat4 m_LocalTransform;
		BoundingVolume m_BoundingVolume;
		bool m_InFrustum;
	};
}