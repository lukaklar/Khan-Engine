#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/gpuvendor.hpp"
#include "graphicshal/vulkan/vulkanadapter.hpp"
#include "graphicshal/vulkan/vulkancore.h"
#include "graphicshal/vulkan/vulkanutils.hpp"

namespace Khan
{
	static const char* s_DesiredExtensions[] =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	DisplayAdapter::DisplayAdapter(VkPhysicalDevice adapter)
		: m_Adapter(adapter)
	{
		vkGetPhysicalDeviceProperties(adapter, &m_Properties);
		vkGetPhysicalDeviceFeatures(adapter, &m_Features);

		uint32_t queueFamilyPropertyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &queueFamilyPropertyCount, queueFamilyProperties.data());

		for (uint32_t i = 0; i < QueueType_Count; ++i)
		{
			m_QueueFamilyIndices[i] = UINT32_MAX;
		}

		for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i)
		{
			if (m_QueueFamilyIndices[QueueType_Graphics] == UINT32_MAX && queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				m_QueueFamilyIndices[QueueType_Graphics] = i;
			}

			if (m_QueueFamilyIndices[QueueType_Compute] == UINT32_MAX && queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && !(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				m_QueueFamilyIndices[QueueType_Compute] = i;
			}

			if (m_QueueFamilyIndices[QueueType_Copy] == UINT32_MAX && queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !(queueFamilyProperties[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
			{
				m_QueueFamilyIndices[QueueType_Copy] = i;
			}
		}

		uint32_t extensionPropertyCount;
		VK_ASSERT(vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extensionPropertyCount, nullptr), "[VULKAN] Failed to get device extension property count.");
		std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
		VK_ASSERT(vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extensionPropertyCount, extensionProperties.data()), "[VULKAN] Failed to enumerate device extension properties.");

		for (const char* extension : s_DesiredExtensions)
		{
			if (CheckExtension(extension, extensionProperties))
			{
				m_SupportedExtensions.push_back(extension);
			}
		}
	}

	GPUVendor DisplayAdapter::GetVendor() const
	{
		switch (m_Properties.vendorID)
		{
		case 0x1002:
			return GPUVendor::AMD;
		case 0x10DE:
			return GPUVendor::Nvidia;
		case 0x8086:
			return GPUVendor::Intel;
		default:
			return GPUVendor::Unknown;
		}
	}
}

#endif // KH_GFXAPI_VULKAN