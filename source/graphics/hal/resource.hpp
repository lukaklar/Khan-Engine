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

		void Update(const void* data, uint32_t size, uint32_t offset);
		inline const uint8_t* GetShadowData() const { return m_ShadowData; }
		inline uint32_t GetDataSize() const { return m_Size; }

		inline bool IsDirty() const { return m_Dirty; }
		inline void SetDirty(bool value) { m_Dirty = value; }

#ifdef KH_DEBUG
		const std::string& GetDebugName() const { return m_DebugName; }
		void SetDebugName(const std::string& debugName) { m_DebugName = debugName; }
#endif // KH_DEBUG

	protected:
		Resource(ResourceState state = ResourceState_Undefined, QueueType queue = QueueType_None);
		virtual ~Resource() = 0;

		ResourceState m_State;
		QueueType m_Queue;

		uint8_t* m_ShadowData;
		uint32_t m_Size;
		bool m_Dirty;

#ifdef KH_DEBUG
		std::string m_DebugName;
#endif // KH_DEBUG
	};
}