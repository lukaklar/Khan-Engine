#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vk_mem_alloc.h"
#include "graphics/hal/vulkan/vulkancore.h"
#include "graphics/hal/vulkan/vulkanutils.hpp"

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

#include "graphics/hal/vulkan/vulkandevicememorymanager.inl"

#endif // KH_GFXAPI_VULKAN