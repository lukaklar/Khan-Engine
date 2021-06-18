#include "core/precomp.h"
#include "core/events/eventlistenerholder.h"
#include "core/events/event.h"
#include "core/events/eventchannel.h"

void EventListenerHolder::Initialize()
{
}

void EventListenerHolder::Shutdown()
{
	for (auto& it : m_Callbacks)
	{
		delete it.second;
	}
	for (EventChannel* channel : m_Channels)
	{
		channel->UnregisterListener(this);
	}
}

void EventListenerHolder::Subscribe(EventChannel& channel)
{
	channel.RegisterListener(this);
	m_Channels.emplace(&channel);
}

void EventListenerHolder::Unsubscribe(EventChannel& channel)
{
	channel.RegisterListener(this);
	m_Channels.erase(&channel);
}

void EventListenerHolder::HandleEvent(const Event& event)
{
	auto it = m_Callbacks.find(typeid(event));
	if (it != m_Callbacks.end())
	{
		it->second->Exec(event);
	}
}