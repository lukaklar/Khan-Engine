#pragma once
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

		std::vector<System*> m_Systems;
	};
}