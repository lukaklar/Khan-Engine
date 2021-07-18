#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/config.h"
#include "graphics/hal/vulkan/vk_mem_alloc.h"

namespace Khan
{
	class VulkanUploadManager
	{
	public:
		void Create(VmaAllocator allocator);
		void Destroy();

		uint32_t Upload(const void* src, uint32_t size);
		void ResetFrame(uint32_t frameIndex);

		VkBuffer CurrentBuffer() const;

	private:
		VmaAllocator m_Allocator;

		struct StagingBuffer
		{
			VkBuffer m_Buffer;
			VmaAllocation m_Allocation;
			uint8_t* m_MappedMemory;
			uint32_t m_Capacity;
			uint32_t m_Offset;
		};

		std::mutex m_Lock;
		std::vector<StagingBuffer*> m_FreeStagingBuffers;
		std::vector<StagingBuffer*> m_UsedStagingBuffers[K_MAX_FRAMES_IN_FLIGHT];
		StagingBuffer* m_CurrentStagingBuffer;
		uint32_t m_CurrentFrameIndex;

		static constexpr uint32_t K_DEFAULT_STAGING_BUFFER_SIZE = 2 * 1024 * 1024;
		static constexpr uint32_t K_STAGING_BUFFER_ALIGNMENT = 4;
	};
}

#include "graphics/hal/vulkan/vulkanuploadmanager.inl"

#endif // KH_GFXAPI_VULKAN