#pragma once
#include "core/events/event.h"
#include "core/events/eventchannel.h"

class WindowEvent : public Event
{
};

class WindowClosedEvent : public WindowEvent
{
};

class WindowCreatedEvent : public WindowEvent
{
public:
	WindowCreatedEvent(void* handle, uint32_t width, uint32_t height)
		: m_Handle(handle), m_Width(width), m_Height(height) {}

	inline void* GetHandle() const { return m_Handle; }
	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }

private:
	void* m_Handle;
	uint32_t m_Width, m_Height;
};

class WindowResizeEvent : public WindowEvent
{
public:
	WindowResizeEvent(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) {}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }

private:
	uint32_t m_Width, m_Height;
};

class WindowChannel : public EventChannel
{
public:
	static WindowChannel& GetChannel();

	void Broadcast(const WindowEvent& event);
};