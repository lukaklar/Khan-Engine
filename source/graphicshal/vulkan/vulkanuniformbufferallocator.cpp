#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkancore.h"
#include "graphicshal/vulkan/vulkanuniformbufferallocator.hpp"

namespace Khan
{
	void VulkanUniformBufferAllocator::Create(VmaAllocator allocator)
	{
		for (FramePool& pool : m_FramePools)
		{
			pool.Create(allocator);
		}
		m_CurrentPool = &m_FramePools[0];
	}

	void VulkanUniformBufferAllocator::Destroy(VmaAllocator allocator)
	{
		for (FramePool& pool : m_FramePools)
		{
			pool.Destroy(allocator);
		}
	}

	inline void VulkanUniformBufferAllocator::FramePool::Create(VmaAllocator allocator)
	{
		VkBufferCreateInfo info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		info.size = K_UNIFORM_BUFFER_SIZE;
		info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VK_ASSERT(vmaCreateBuffer(allocator, &info, &allocInfo, &m_Buffer, &m_Allocation, nullptr), "[VULKAN] Failed to allocate uniform buffer.");

		VK_ASSERT(vmaMapMemory(allocator, m_Allocation, reinterpret_cast<void**>(&m_MappedMemory)), "[VULKAN] Failed to map uniform buffer memory.");

		m_Offset.store(0);
	}

	inline void VulkanUniformBufferAllocator::FramePool::Destroy(VmaAllocator allocator)
	{
		vmaUnmapMemory(allocator, m_Allocation);
		vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
	}
}

#endif // KH_GFXAPI_VULKAN