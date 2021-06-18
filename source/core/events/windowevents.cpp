#include "core/precomp.h"
#include "core/events/windowevents.h"

WindowChannel s_WindowChannel;

WindowChannel& WindowChannel::GetChannel()
{
	return s_WindowChannel;
}

void WindowChannel::Broadcast(const WindowEvent& event)
{
	EventChannel::Broadcast(event);
}