#pragma once

#include "system/assert.h"

namespace Khan
{
	KH_FORCE_INLINE void ConstantBuffer::UpdateConstantData(const void* data, uint32_t offset, uint32_t size)
	{
		KH_ASSERT(offset + size <= m_Size, "Constant buffer overflow.");
		std::memcpy(m_ShadowData + offset, data, size);
		m_IsDirty = true;
	}

	KH_FORCE_INLINE const void* ConstantBuffer::Data() const
	{
		return m_ShadowData;
	}

	KH_FORCE_INLINE uint32_t ConstantBuffer::Size() const
	{
		return m_Size;
	}

	KH_FORCE_INLINE uint32_t ConstantBuffer::GetDynamicOffset() const
	{
		return m_DynamicOffset;
	}

	KH_FORCE_INLINE void ConstantBuffer::SetDynamicOffset(uint32_t offset)
	{
		m_DynamicOffset = offset;
	}

	KH_FORCE_INLINE bool ConstantBuffer::IsDirty() const
	{
		return m_IsDirty;
	}
}