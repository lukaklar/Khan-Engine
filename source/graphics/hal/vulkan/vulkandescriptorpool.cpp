#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkandescriptorpool.hpp"

namespace Khan
{
	VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device)
		: m_Device(device)
		, m_CurrentPool(VK_NULL_HANDLE)
		, m_CurrentFrameIndex(0)
	{
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		for (uint32_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_FreePools.insert(m_FreePools.end(), m_UsedPools[i].begin(), m_UsedPools[i].end());
		}
		for (VkDescriptorPool pool : m_FreePools)
		{
			vkDestroyDescriptorPool(m_Device, pool, nullptr);
		}
	}
}

#endif // KH_GFXAPI_VULKAN