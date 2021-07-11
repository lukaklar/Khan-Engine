#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/transientresourcemanager.hpp"
#include "graphics/hal/vulkan/vulkanbuffer.hpp"
#include "graphics/hal/vulkan/vulkanbufferview.hpp"
#include "graphics/hal/vulkan/vulkantexture.hpp"
#include "graphics/hal/vulkan/vulkantextureview.hpp"

namespace Khan
{
	class VulkanTransientResourceManager : public TransientResourceManager
	{
	public:
		VulkanTransientResourceManager(RenderDevice& device);

		void Create(VmaAllocator allocator);
		void Destroy();

		virtual Buffer* FindOrCreateBuffer(const RenderPass* pass, const BufferDesc& desc) override;
		virtual BufferView* FindOrCreateBufferView(const RenderPass* pass, Buffer* buffer, const BufferViewDesc& desc) override;
		virtual Texture* FindOrCreateTexture(const RenderPass* pass, const TextureDesc& desc) override;
		virtual TextureView* FindOrCreateTextureView(const RenderPass* pass, Texture* texture, const TextureViewDesc& desc) override;

		void ResetFrame(uint32_t frameIndex);

	private:
		VmaAllocator m_Allocator;

		VulkanBuffer m_BufferPool[K_BUFFER_POOL_SIZE];
		VulkanTexture m_TexturePool[K_TEXTURE_POOL_SIZE];

		VulkanBuffer m_DummyBufferPool[K_BUFFER_VIEW_POOL_SIZE];
		VulkanBufferView m_BufferViewPool[K_BUFFER_VIEW_POOL_SIZE];
		VulkanTexture m_DummyTexturePool[K_TEXTURE_VIEW_POOL_SIZE];
		VulkanTextureView m_TextureViewPool[K_TEXTURE_VIEW_POOL_SIZE];
	};
}

#endif // KH_GFXAPI_VULKAN