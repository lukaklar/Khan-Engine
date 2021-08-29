#pragma once
#include "graphics/hal/queuetype.hpp"
#include "graphics/hal/resourcestate.hpp"

namespace Khan
{
	class Resource
	{
	public:
		inline ResourceState GetState() const { return m_State; }
		inline void SetState(ResourceState state) { m_State = state; }
		inline QueueType GetQueue() const { return m_Queue; }
		inline void SetQueue(QueueType queue) { m_Queue = queue; }

#ifdef KH_DEBUG
		inline const std::string& GetDebugName() const { return m_DebugName; }
		inline void SetDebugName(const std::string& debugName) { m_DebugName = debugName; }
#endif // KH_DEBUG

	protected:
		Resource(ResourceState state = ResourceState_Undefined, QueueType queue = QueueType_None)
			: m_State(state)
			, m_Queue(queue)
		{
		}

		virtual ~Resource() = 0;

		ResourceState m_State;
		QueueType m_Queue;

#ifdef KH_DEBUG
		std::string m_DebugName;
#endif // KH_DEBUG
	};
}