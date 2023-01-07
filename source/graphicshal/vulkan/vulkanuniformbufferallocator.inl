#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "system/assert.h"

namespace Khan
{
	KH_FORCE_INLINE uint32_t VulkanUniformBufferAllocator::Upload(const void* src, uint32_t size)
	{
		static const uint32_t alignment = 256; //static_cast<uint32_t>(Device::ms_DeviceProperties.limits.minUniformBufferOffsetAlignment);
		uint32_t paddedSize = (size + alignment - 1) & ~(alignment - 1);
		uint32_t offset = m_CurrentPool->m_Offset.fetch_add(paddedSize);
		KH_ASSERT(offset + paddedSize <= K_UNIFORM_BUFFER_SIZE, "[D3D12] Uniform buffer pool is too small.");
		std::memcpy(m_CurrentPool->m_MappedMemory + offset, src, size);
		return offset;
	}

	KH_FORCE_INLINE void VulkanUniformBufferAllocator::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentPool = &m_FramePools[frameIndex];
		m_CurrentPool->m_Offset.store(0);
	}

	KH_FORCE_INLINE VkBuffer VulkanUniformBufferAllocator::CurrentBuffer() const
	{
		return m_CurrentPool->m_Buffer;
	}
}

#endif // KH_GFXAPI_VULKAN