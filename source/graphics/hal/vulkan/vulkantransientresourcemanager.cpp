#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkantransientresourcemanager.hpp"
#include "system/assert.h"

namespace Khan
{
	VulkanTransientResourceManager::VulkanTransientResourceManager(RenderDevice& device)
		: TransientResourceManager(device)
	{
	}

	void VulkanTransientResourceManager::Create(VmaAllocator allocator)
	{
		m_Allocator = allocator;
	}

	void VulkanTransientResourceManager::Destroy()
	{
	}

	Buffer* VulkanTransientResourceManager::FindOrCreateBuffer(const RenderPass* pass, const BufferDesc& desc)
	{
		/*std::pair key(pass, desc);
		auto it = m_DescToBufferMap.find(key);
		if (it != m_DescToBufferMap.end())
		{
			return it->second;
		}*/

		KH_ASSERT(m_BufferPoolIndex < K_BUFFER_POOL_SIZE, "Buffer pool too small, you should increase its size.");
		Buffer* value = new(&m_BufferPool[m_BufferPoolIndex++])VulkanBuffer(VK_NULL_HANDLE, nullptr, desc);
		//m_DescToBufferMap.insert({ key, value });

		return value;
	}

	BufferView* VulkanTransientResourceManager::FindOrCreateBufferView(const RenderPass* pass, Buffer* buffer, const BufferViewDesc& desc)
	{
		std::pair<const RenderPass*, std::pair<Buffer*, BufferViewDesc>> key(pass, { buffer, desc });
		auto it = m_DescToBufferViewMap.find(key);
		if (it != m_DescToBufferViewMap.end())
		{
			return it->second;
		}

		KH_ASSERT(m_BufferViewPoolIndex[m_CurrentFrameIndex] < K_BUFFER_VIEW_POOL_SIZE, "Buffer view pool too small, you should increase its size.");
		VulkanBuffer* dummyBuffer = new(&m_DummyBufferPool[m_CurrentFrameIndex][m_BufferViewPoolIndex[m_CurrentFrameIndex]])VulkanBuffer(*reinterpret_cast<VulkanBuffer*>(buffer));
		VulkanBufferView* value = new(&m_BufferViewPool[m_CurrentFrameIndex][m_BufferViewPoolIndex[m_CurrentFrameIndex]++])VulkanBufferView(VK_NULL_HANDLE, *buffer, desc);
		m_DescToBufferViewMap.insert({ key, value });

		return value;
	}

	Texture* VulkanTransientResourceManager::FindOrCreateTexture(const RenderPass* pass, const TextureDesc& desc)
	{
		/*std::pair key(pass, desc);
		auto it = m_DescToTextureMap.find(key);
		if (it != m_DescToTextureMap.end())
		{
			return it->second;
		}*/

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

		KH_ASSERT(m_TexturePoolIndex < K_TEXTURE_POOL_SIZE, "Texture pool too small, you should increase its size.");
		VulkanTexture* value = new(&m_TexturePool[m_TexturePoolIndex++])VulkanTexture(image, allocation, desc);
		//m_DescToTextureMap.insert({ key, value });

		return value;
	}

	TextureView* VulkanTransientResourceManager::FindOrCreateTextureView(const RenderPass* pass, Texture* texture, const TextureViewDesc& desc)
	{
		/*std::pair<const RenderPass*, std::pair<Texture*, TextureViewDesc>> key(pass, { texture, desc });
		auto it = m_DescToTextureViewMap.find(key);
		if (it != m_DescToTextureViewMap.end())
		{
			return it->second;
		}*/

		VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = reinterpret_cast<VulkanTexture*>(texture)->VulkanImage();
		viewInfo.viewType = TextureViewTypeToVulkanImageViewType(desc.m_Type);
		viewInfo.format = PixelFormatToVulkanFormat(desc.m_Format);
		viewInfo.subresourceRange.aspectMask = PixelFormatToVulkanAspectMask(desc.m_Format);
		viewInfo.subresourceRange.baseMipLevel = desc.m_BaseMipLevel;
		viewInfo.subresourceRange.levelCount = desc.m_LevelCount;
		viewInfo.subresourceRange.baseArrayLayer = desc.m_BaseArrayLayer;
		viewInfo.subresourceRange.layerCount = desc.m_LayerCount;

		VkImageView imageView;
		VK_ASSERT(vkCreateImageView(m_Device.VulkanDevice(), &viewInfo, nullptr, &imageView), "[VULKAN] Failed to create image view.");

		KH_ASSERT(m_TextureViewPoolIndex[m_CurrentFrameIndex] < K_TEXTURE_VIEW_POOL_SIZE, "Texture view pool too small, you should increase its size.");
		VulkanTexture* dummyTexture = new(&m_DummyTexturePool[m_CurrentFrameIndex][m_TextureViewPoolIndex[m_CurrentFrameIndex]])VulkanTexture(*reinterpret_cast<VulkanTexture*>(texture));
		VulkanTextureView* value = new(&m_TextureViewPool[m_CurrentFrameIndex][m_TextureViewPoolIndex[m_CurrentFrameIndex]++])VulkanTextureView(imageView, *texture, desc);
		//m_DescToTextureViewMap.insert({ key, value });

		return value;
	}

	void VulkanTransientResourceManager::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;

		for (uint32_t i = 0; i < m_BufferViewPoolIndex[frameIndex]; ++i)
		{
			vkDestroyBufferView(m_Device.VulkanDevice(), m_BufferViewPool[frameIndex][i].GetVulkanBufferView(), nullptr);
			m_DummyBufferPool[frameIndex][i].~VulkanBuffer();
			m_BufferViewPool[frameIndex][i].~VulkanBufferView();
		}
		m_BufferViewPoolIndex[frameIndex] = 0;

		for (uint32_t i = 0; i < m_BufferPoolIndex; ++i)
		{
			vmaDestroyBuffer(m_Allocator, m_BufferPool[i].GetVulkanBuffer(), m_BufferPool[i].VulkanAllocation());
			m_BufferPool[i].~VulkanBuffer();
		}
		m_BufferPoolIndex = 0;

		for (uint32_t i = 0; i < m_TextureViewPoolIndex[frameIndex]; ++i)
		{
			vkDestroyImageView(m_Device.VulkanDevice(), m_TextureViewPool[frameIndex][i].VulkanImageView(), nullptr);
			m_DummyTexturePool[frameIndex][i].~VulkanTexture();
			m_TextureViewPool[frameIndex][i].~VulkanTextureView();
		}
		m_TextureViewPoolIndex[frameIndex] = 0;

		for (uint32_t i = 0; i < m_TexturePoolIndex; ++i)
		{
			vmaDestroyImage(m_Allocator, m_TexturePool[i].VulkanImage(), m_TexturePool[i].VulkanAllocation());
			m_TexturePool[i].~VulkanTexture();
		}
		m_TexturePoolIndex = 0;
	}
}

#endif // KH_GFXAPI_VULKAN