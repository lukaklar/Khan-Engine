#pragma once
#include "core/core_dll.h"

class Event;
class EventChannel;

class CallbackFunctionBase
{
public:
	virtual ~CallbackFunctionBase() {}

	void Exec(const Event& event) { Call(event); }

protected:
	virtual void Call(const Event&) = 0;
};

template<class T, class EventT>
class MemberFunctionCallback : public CallbackFunctionBase
{
public:
	typedef void (T::*MemberFunc)(const EventT&);

	MemberFunctionCallback(T* instance, MemberFunc memFn)
		: m_Instance(instance), m_Function(memFn)
	{
	}

private:
	virtual void Call(const Event& event) override
	{
		(m_Instance->*m_Function)(static_cast<const EventT&>(event));
	}

private:
	T* m_Instance;
	MemberFunc m_Function;
};

class KH_CORE_DLL EventListenerHolder
{
public:
	void Initialize();
	void Shutdown();

	template<class T, class EventT>
	void RegisterCallback(T* obj, void (T::*memFn)(const EventT&))
	{
		m_Callbacks[typeid(EventT)] = new MemberFunctionCallback<T, EventT>(obj, memFn);
	}

	template<class T, class EventT>
	void UnregisterCallback(void (T::*memFn)(const EventT&))
	{
		delete m_Callbacks[typeid(EventT)];
		m_Callbacks.erase(typeid(EventT));
	}

	void Subscribe(EventChannel& channel);
	void Unsubscribe(EventChannel& channel);

private:
	void HandleEvent(const Event& event);

private:
	friend class EventChannel;

	std::unordered_map<std::type_index, CallbackFunctionBase*> m_Callbacks;
	std::unordered_set<EventChannel*> m_Channels;
};