#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "graphicshal/transientresourcemanager.hpp"
#include "graphicshal/vulkan/vulkanbuffer.hpp"
#include "graphicshal/vulkan/vulkanbufferview.hpp"
#include "graphicshal/vulkan/vulkantexture.hpp"
#include "graphicshal/vulkan/vulkantextureview.hpp"

namespace Khan
{
	class VulkanRenderDevice;

	class VulkanTransientResourceManager : public TransientResourceManager
	{
	public:
		VulkanTransientResourceManager(RenderDevice& device);

		void Create(VmaAllocator allocator);
		void Destroy();

		virtual RenderDevice& GetDevice() const override;

		virtual Buffer* FindOrCreateBuffer(const RenderPass* pass, const BufferDesc& desc, uint32 resourceIndex) override;
		virtual BufferView* FindOrCreateBufferView(Buffer* buffer, const BufferViewDesc& desc) override;
		virtual Texture* FindOrCreateTexture(const RenderPass* pass, const TextureDesc& desc, uint32 resourceIndex) override;
		virtual TextureView* FindOrCreateTextureView(Texture* texture, const TextureViewDesc& desc) override;

		void ResetFrame(uint32 frameIndex);

	private:
		VulkanRenderDevice& m_Device;

		VmaAllocator m_Allocator;

		VulkanBuffer m_BufferPool[K_BUFFER_POOL_SIZE];
		VulkanTexture m_TexturePool[K_TEXTURE_POOL_SIZE];

		THashMap<TPair<Buffer*, BufferViewDesc>, VkBufferView, pair_hash> m_BufferViewMap;
		THashMap<TPair<Texture*, TextureViewDesc>, VkImageView, pair_hash> m_ImageViewMap;

		VulkanBuffer m_DummyBufferPool[K_MAX_FRAMES_IN_FLIGHT][K_BUFFER_VIEW_POOL_SIZE];
		VulkanBufferView m_BufferViewPool[K_MAX_FRAMES_IN_FLIGHT][K_BUFFER_VIEW_POOL_SIZE];
		VulkanTexture m_DummyTexturePool[K_MAX_FRAMES_IN_FLIGHT][K_TEXTURE_VIEW_POOL_SIZE];
		VulkanTextureView m_TextureViewPool[K_MAX_FRAMES_IN_FLIGHT][K_TEXTURE_VIEW_POOL_SIZE];

		uint32 m_CurrentFrameIndex = 0;
	};
}

#endif // KH_GFXAPI_VULKAN