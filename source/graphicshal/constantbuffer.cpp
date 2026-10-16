#include "graphicshal/precomp.h"
#include "graphicshal/constantbuffer.hpp"
#include "core/primitivetypes.h"

namespace Khan
{
	ConstantBuffer::ConstantBuffer(uint32 size)
		: m_Size(size)
		, m_IsDirty(true)
	{
		m_ShadowData = new byte[size];
	}

	ConstantBuffer::~ConstantBuffer()
	{
		delete m_ShadowData;
	}
}