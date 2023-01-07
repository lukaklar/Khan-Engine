#pragma once

#ifdef KH_GFXAPI_VULKAN

namespace Khan
{
	KH_FORCE_INLINE VulkanBuffer* VulkanDeviceMemoryManager::CreateBuffer(const BufferDesc& desc)
	{
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = desc.m_Size;
		bufferInfo.usage = BufferFlagsToVulkanBufferUsageFlags(desc.m_Flags);

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkBuffer buffer;
		VmaAllocation allocation;
		VK_ASSERT(vmaCreateBuffer(m_Allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr), "[VULKAN] Failed to create buffer.");

		return new VulkanBuffer(buffer, allocation, desc);
	}

	KH_FORCE_INLINE VulkanTexture* VulkanDeviceMemoryManager::CreateTexture(const TextureDesc& desc)
	{
		VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.flags = desc.m_Flags & TextureFlag_CubeMap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
		imageInfo.imageType = TextureTypeToVulkanImageType(desc.m_Type);
		imageInfo.format = PixelFormatToVulkanFormat(desc.m_Format);
		imageInfo.extent = { desc.m_Width, desc.m_Height, desc.m_Depth };
		imageInfo.mipLevels = desc.m_MipLevels;
		imageInfo.arrayLayers = desc.m_ArrayLayers * (desc.m_Flags & TextureFlag_CubeMap ? 6 : 1);
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = desc.m_Type == TextureType_1D ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = TextureFlagsToVulkanImageUsageFlags(desc.m_Flags);
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkImage image;
		VmaAllocation allocation;
		VK_ASSERT(vmaCreateImage(m_Allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr), "[VULKAN] Failed to create image.");

		return new VulkanTexture(image, allocation, desc);
	}

	KH_FORCE_INLINE void VulkanDeviceMemoryManager::DestroyBuffer(VulkanBuffer* buffer)
	{
		vmaDestroyBuffer(m_Allocator, buffer->GetVulkanBuffer(), buffer->VulkanAllocation());
		delete buffer;
	}

	KH_FORCE_INLINE void VulkanDeviceMemoryManager::DestroyTexture(VulkanTexture* texture)
	{
		vmaDestroyImage(m_Allocator, texture->VulkanImage(), texture->VulkanAllocation());
		delete texture;
	}
}

#endif // KH_GFXAPI_VULKAN