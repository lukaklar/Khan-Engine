#include "core/precomp.h"
#include "core/events/eventchannel.h"
#include "core/events/event.h"
#include "core/events/eventlistenerholder.h"

void EventChannel::Broadcast(const Event& event)
{
	for (EventListenerHolder* handler : m_Listeners)
	{
		handler->HandleEvent(event);
	}
}

void EventChannel::RegisterListener(EventListenerHolder* listener)
{
	m_Listeners.emplace(listener);
}

void EventChannel::UnregisterListener(EventListenerHolder* listener)
{
	m_Listeners.erase(listener);
}