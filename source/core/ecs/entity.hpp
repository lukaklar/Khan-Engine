#pragma once
#include "core/boundingvolume.h"
#include "core/containers.h"
#include "core/primitivetypes.h"
#include "core/string.h"
#include "core/typetraits.h"
#include <thirdparty/entt/entt.hpp>

namespace Khan
{
	struct Component;

	class Entity
	{
		friend class World;
	public:
		template<typename TComponent, typename... TArgs>
		TComponent& AddComponent(TArgs&&... args)
		{
			static_assert(IsBaseOf<Component, TComponent>, "New components need to inherit from the Component base class.");
			TComponent& component = m_World->m_Registry.emplace<TComponent>(m_EntityHandle, Forward<TArgs>(args)...);
			component.m_Owner = this;
			m_Components.emplace_back(&component);
			return component;
		}

		template<typename TComponent>
		TComponent& GetComponent()
		{
			return m_World->m_Registry.get<TComponent>(m_EntityHandle);
		}

#ifdef KH_DEBUG
		template<typename TComponent>
		bool HasComponent()
		{
			return m_World->m_Registry.all_of<TComponent>(m_EntityHandle);
		}
#endif // KH_DEBUG

		template<typename TComponent>
		void RemoveComponent()
		{
			Component* component = &m_World->m_Registry.get<TComponent>(m_EntityHandle);
			m_World->m_Registry.remove<TComponent>(m_EntityHandle);
			m_Components.erase(Find(m_Components.begin(), m_Components.end(), component));
		}

		void AddChild(Entity* child);
		void RemoveChild(Entity* child);

		inline bool IsValid() const { return m_EntityHandle != entt::null; }
		inline uint32 GetID() const { return static_cast<uint32>(m_EntityHandle); }
		inline World* GetWorld() const { return m_World; }
		inline Entity* GetParent() const { return m_ParentEntity; }
		inline void SetParent(Entity* value) { m_ParentEntity = value; }
		inline const TSet<Entity*>& GetChildren() const { return m_Children; }
		inline const TArray<Component*>& GetComponents() const { return m_Components; }
		inline const String& GetName() const { return m_Name; }
		inline void SetName(const String& value) { m_Name = value; }
		inline const Vector4& GetGlobalPosition() const { return m_GlobalPosition; }
		inline void SetGlobalPosition(const Vector4& value) { m_GlobalPosition = value; }
		inline const Quaternion& GetGlobalOrientation() const { return m_GlobalOrientation; }
		inline void SetGlobalOrientation(const Quaternion& value) { m_GlobalOrientation = value; }
		inline const Matrix4& GetGlobalTransform() const { return m_GlobalTransform; }
		inline void SetGlobalTransform(const Matrix4& value) { m_GlobalTransform = value; }
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
		TSet<Entity*> m_Children;
		TArray<Component*> m_Components;
		String m_Name = "Empty Entity";
		Vector4 m_GlobalPosition;
		Quaternion m_GlobalOrientation;
		Matrix4 m_GlobalTransform;
		//Vector4 m_LocalPosition;
		//Quaternion m_LocalOrientation;
		//Matrix4 m_LocalTransform;
		BoundingVolume m_BoundingVolume;
		bool m_InFrustum;
	};
}