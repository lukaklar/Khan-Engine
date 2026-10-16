#pragma once
#include "core/containers.h"
#include "core/singleton.h"

namespace Khan
{
	class System;

	class SystemManager : public Singleton<SystemManager>
	{
		friend class Singleton<SystemManager>;
	public:
		void AddSystem(System* system);
		void UpdateSystems(float dt) const;

	private:
		SystemManager() = default;
		~SystemManager();

		TArray<System*> m_Systems;
	};
}