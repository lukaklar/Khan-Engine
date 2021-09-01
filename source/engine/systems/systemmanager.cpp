#include "engine/precomp.h"
#include "engine/systems/systemmanager.hpp"
#include "core/ecs/system.hpp"

namespace Khan
{
	SystemManager::~SystemManager()
	{
		for (System* system : m_Systems)
		{
			delete system;
		}
	}

	void SystemManager::AddSystem(System* system)
	{
		m_Systems.push_back(system);
	}

	void SystemManager::UpdateSystems(float dt) const
	{
		for (System* system : m_Systems)
		{
			system->Update(dt);
		}
	}
}