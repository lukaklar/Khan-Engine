#pragma once
#include "core/containers.h"
#include "core/core_dll.h"

namespace Khan
{
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

	template<class TClass, class TEvent>
	class MemberFunctionCallback : public CallbackFunctionBase
	{
	public:
		typedef void (TClass::* TMethod)(const TEvent&);

		MemberFunctionCallback(TClass* object, TMethod&& method)
			: m_Object(object), m_Method(method)
		{
		}

	private:
		virtual void Call(const Event& event) override
		{
			(m_Object->*m_Function)(static_cast<const TEvent&>(event));
		}

	private:
		TClass* m_Object;
		TMethod m_Method;
	};

	class KH_CORE_DLL EventListenerHolder
	{
	public:
		void Initialize();
		void Shutdown();

		template<class TClass, class TEvent>
		void RegisterCallback(TClass* obj, void (TClass::* method)(const TEvent&))
		{
			m_Callbacks[typeid(TEvent)] = new MemberFunctionCallback<TClass, TEvent>(obj, method);
		}

		template<class TClass, class TEvent>
		void UnregisterCallback(void (TClass::* method)(const TEvent&))
		{
			delete m_Callbacks[typeid(TEvent)];
			m_Callbacks.erase(typeid(TEvent));
		}

		void Subscribe(EventChannel& channel);
		void Unsubscribe(EventChannel& channel);

	private:
		void HandleEvent(const Event& event);

	private:
		friend class EventChannel;

		THashMap<std::type_index, CallbackFunctionBase*> m_Callbacks;
		THashSet<EventChannel*> m_Channels;
	};
}