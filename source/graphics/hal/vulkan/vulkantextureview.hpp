#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/textureview.hpp"
#include <vulkan/vulkan.h>

namespace Khan
{
	class VulkanTextureView : public TextureView
	{
	public:
		VulkanTextureView(VkImageView view, Texture& texture, const TextureViewDesc& desc)
			: TextureView(texture, desc)
			, m_ImageView(view)
		{
		}

		inline VkImageView VulkanImageView() const { return m_ImageView; }

	protected:
		friend class VulkanTransientResourceManager;
		VulkanTextureView() = default;

	private:
		VkImageView m_ImageView;
	};
}

#endif // KH_GFXAPI_VULKAN