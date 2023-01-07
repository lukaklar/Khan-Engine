#include "graphicshal/precomp.h"
#include "graphicshal/constantbuffer.hpp"

namespace Khan
{
	ConstantBuffer::ConstantBuffer(uint32_t size)
		: m_Size(size)
		, m_IsDirty(true)
	{
		m_ShadowData = new uint8_t[size];
	}

	ConstantBuffer::~ConstantBuffer()
	{
		delete m_ShadowData;
	}
}