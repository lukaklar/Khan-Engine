#pragma once
#include "core/core_dll.h"

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

	std::unordered_set<EventListenerHolder*> m_Listeners;
};