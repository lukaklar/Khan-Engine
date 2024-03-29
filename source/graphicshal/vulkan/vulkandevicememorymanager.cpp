#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkanbackend.hpp"
#include "graphicshal/vulkan/vulkandevicememorymanager.hpp"

namespace Khan
{
	void VulkanDeviceMemoryManager::Create(VkPhysicalDevice physicalDevice, VkDevice device)
	{
		VmaAllocatorCreateInfo info = {};
		info.physicalDevice = physicalDevice;
		info.device = device;
		info.instance = RenderBackend::g_Instance;

		VK_ASSERT(vmaCreateAllocator(&info, &m_Allocator), "[VULKAN] Failed to create memory allocator.");
	}

	void VulkanDeviceMemoryManager::Destroy()
	{
		vmaDestroyAllocator(m_Allocator);
	}
}

#endif // KH_GFXAPI_VULKAN