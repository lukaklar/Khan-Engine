#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkancore.h"

namespace Khan
{
	KH_FORCE_INLINE VkCommandBuffer VulkanCommandPool::AllocateCommandBuffer()
	{
		if (m_CurrentPool->m_Index < m_CurrentPool->m_CommandBuffers.size())
		{
			return m_CurrentPool->m_CommandBuffers[m_CurrentPool->m_Index++];
		}
		else
		{
			VkCommandBufferAllocateInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			info.commandPool = m_CurrentPool->m_CommandPool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandBufferCount = 1;

			VkCommandBuffer buffer;
			VK_ASSERT(vkAllocateCommandBuffers(m_Device, &info, &buffer), "[VULKAN] Failed to allocate command buffer.");

			m_CurrentPool->m_CommandBuffers.push_back(buffer);
			++m_CurrentPool->m_Index;

			return buffer;
		}
	}

	KH_FORCE_INLINE void VulkanCommandPool::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentPool = &m_FramePools[frameIndex];
		VK_ASSERT(vkResetCommandPool(m_Device, m_CurrentPool->m_CommandPool, 0), "[VULKAN] Failed to reset command pool.");
	}
}

#endif // KH_GFXAPI_VULKAN