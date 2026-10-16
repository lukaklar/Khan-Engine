#pragma once
#include "core/containers.h"
#include "core/core_dll.h"

namespace Khan
{
	class Event;
	class EventListenerHolder;

	class KH_CORE_DLL EventChannel
	{
	protected:
		void Broadcast(const Event& event);

	private:
		void RegisterListener(EventListenerHolder* listener);
		void UnregisterListener(EventListenerHolder* listener);

	private:
		friend class EventListenerHolder;

		THashSet<EventListenerHolder*> m_Listeners;
	};
}