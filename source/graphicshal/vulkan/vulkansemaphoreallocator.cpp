#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkansemaphoreallocator.hpp"

namespace Khan
{
	void VulkanSemaphoreAllocator::Create(VkDevice device)
	{
		m_Device = device;
		m_CurrentFrameIndex = 0;
	}

	void VulkanSemaphoreAllocator::Destroy()
	{
		for (uint32_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_FreeSemaphores.insert(m_FreeSemaphores.end(), m_UsedSemaphores[i].begin(), m_UsedSemaphores[i].end());
		}
		for (VkSemaphore semaphore : m_FreeSemaphores)
		{
			vkDestroySemaphore(m_Device, semaphore, nullptr);
		}
	}
}

#endif // KH_GFXAPI_VULKAN