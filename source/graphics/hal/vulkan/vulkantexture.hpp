#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/texture.hpp"
#include "graphics/hal/vulkan/vk_mem_alloc.h"

namespace Khan
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(VkImage image, VmaAllocation allocation, const TextureDesc& desc)
			: Texture(desc)
			, m_Image(image)
			, m_Allocation(allocation)
		{
		}

		inline VkImage VulkanImage() const { return m_Image; }
		inline VmaAllocation VulkanAllocation() const { return m_Allocation; }

	protected:
		friend class VulkanTransientResourceManager;
		VulkanTexture() = default;

	private:
		VkImage m_Image;
		VmaAllocation m_Allocation;
	};
}

#endif // KH_GFXAPI_VULKAN