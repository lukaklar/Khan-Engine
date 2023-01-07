#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkancore.h"

namespace Khan
{
	KH_FORCE_INLINE VkSemaphore VulkanSemaphoreAllocator::AllocateSemaphore()
	{
		VkSemaphore semaphore;

		if (m_FreeSemaphores.empty())
		{
			VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			VK_ASSERT(vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &semaphore), "[VULKAN] Failed to create semaphore.");
		}
		else
		{
			semaphore = m_FreeSemaphores.back();
			m_FreeSemaphores.pop_back();
		}

		m_UsedSemaphores[m_CurrentFrameIndex].push_back(semaphore);

		return semaphore;
	}

	KH_FORCE_INLINE void VulkanSemaphoreAllocator::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;
		m_FreeSemaphores.insert(m_FreeSemaphores.end(), m_UsedSemaphores[frameIndex].begin(), m_UsedSemaphores[frameIndex].end());
		m_UsedSemaphores[frameIndex].clear();
	}
}

#endif // KH_GFXAPI_VULKAN