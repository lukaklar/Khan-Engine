#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/buffer.hpp"
#include "graphicshal/vulkan/vk_mem_alloc.h"

namespace Khan
{
	class VulkanBuffer : public Buffer
	{
	public:
		VulkanBuffer(VkBuffer buffer, VmaAllocation allocation, const BufferDesc& desc)
			: Buffer(desc)
			, m_Buffer(buffer)
			, m_Allocation(allocation)
		{
		}

		inline VkBuffer GetVulkanBuffer() const { return m_Buffer; }
		inline VmaAllocation VulkanAllocation() const { return m_Allocation; }

	protected:
		friend class VulkanTransientResourceManager;
		VulkanBuffer() = default;

	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;
	};
}

#endif // KH_GFXAPI_VULKAN