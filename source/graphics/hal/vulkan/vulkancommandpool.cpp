#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkancommandpool.hpp"

namespace Khan
{

	void VulkanCommandPool::Create(VkDevice device, uint32_t queueFamilyIndex)
	{
		m_Device = device;

		VkCommandPoolCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		info.queueFamilyIndex = queueFamilyIndex;

		for (FramePool& pool : m_FramePools)
		{
			VK_ASSERT(vkCreateCommandPool(device, &info, nullptr, &pool.m_CommandPool), "[VULKAN] Failed to create command pool.");
			pool.m_Index = 0;
		}

		m_CurrentPool = &m_FramePools[0];
	}

	void VulkanCommandPool::Destroy()
	{
		for (FramePool& pool : m_FramePools)
		{
			vkDestroyCommandPool(m_Device, pool.m_CommandPool, nullptr);
		}
	}
}

#endif // KH_GFXAPI_VULKAN