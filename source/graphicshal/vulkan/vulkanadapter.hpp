#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "core/primitivetypes.h"
#include "graphicshal/queuetype.hpp"

namespace Khan
{
	enum GPUVendor;

	class DisplayAdapter
	{
	public:
		DisplayAdapter(VkPhysicalDevice adapter);

		VkPhysicalDevice VulkanAdapter() const { return m_Adapter; }

		GPUVendor GetVendor() const;
		inline const char* Name() const { return m_Properties.deviceName; }
		inline bool IsDiscrete() const { return m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU; }
		inline bool IsIntegrated() const { return m_Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU; }
		inline const VkPhysicalDeviceFeatures& GetEnabledFeatures() const { return m_Features; }
		//inline bool SupportsAsyncCompute() const { return m_QueueFamilyIndices[QueueType_Compute] != UINT32_MAX; }
		//inline bool HasDMA() const { m_QueueFamilyIndices[QueueType_Copy] != UINT32_MAX; }
		inline const uint32* GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		inline const TArray<const char*>& GetSupportedExtensions() const { return m_SupportedExtensions; }

	private:
		VkPhysicalDevice m_Adapter;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		uint32 m_QueueFamilyIndices[QueueType_Count];
		TArray<const char*> m_SupportedExtensions;
	};
}

#endif // KH_GFXAPI_VULKAN