#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vk_mem_alloc.h"
#include "graphicshal/vulkan/vulkancore.h"
#include "graphicshal/vulkan/vulkanutils.hpp"

namespace Khan
{
	class VulkanDeviceMemoryManager
	{
	public:
		void Create(VkPhysicalDevice physicalDevice, VkDevice device);
		void Destroy();

		VmaAllocator VulkanMemoryAllocator() const { return m_Allocator; }

		VulkanBuffer* CreateBuffer(const BufferDesc& desc);
		VulkanTexture* CreateTexture(const TextureDesc& desc);

		void DestroyBuffer(VulkanBuffer* buffer);
		void DestroyTexture(VulkanTexture* texture);

	private:
		VmaAllocator m_Allocator;
	};
}

#include "graphicshal/vulkan/vulkandevicememorymanager.inl"

#endif // KH_GFXAPI_VULKAN