#include "graphics/precomp.h"
#include "graphics/hal/resource.hpp"

namespace Khan
{
	Resource::Resource(ResourceState state, QueueType queue)
		: m_State(state)
		, m_Queue(queue)
		, m_ShadowData(nullptr)
		, m_Size(0)
		, m_Dirty(false)
	{
	}

	Resource::~Resource()
	{
		delete m_ShadowData;
	}

	void Resource::Update(const void* data, uint32_t size, uint32_t offset)
	{
		uint32_t newSize = offset + size;
		if (newSize > m_Size)
		{
			m_ShadowData = new uint8_t[newSize];
			m_Size = newSize;
		}

		std::memcpy(m_ShadowData + offset, data, size);
		m_Dirty = true;
	}
}