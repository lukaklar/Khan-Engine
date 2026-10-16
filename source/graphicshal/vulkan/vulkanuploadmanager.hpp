#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "core/primitivetypes.h"
#include "graphicshal/config.h"
#include "graphicshal/vulkan/vk_mem_alloc.h"

namespace Khan
{
	class VulkanUploadManager
	{
	public:
		void Create(VmaAllocator allocator);
		void Destroy();

		uint32 Upload(const void* src, uint32 size);
		void ResetFrame(uint32 frameIndex);

		VkBuffer CurrentBuffer() const;

	private:
		VmaAllocator m_Allocator;

		struct StagingBuffer
		{
			VkBuffer m_Buffer;
			VmaAllocation m_Allocation;
			byte* m_MappedMemory;
			uint32 m_Capacity;
			uint32 m_Offset;
		};

		std::mutex m_Lock;
		TArray<StagingBuffer*> m_FreeStagingBuffers;
		TArray<StagingBuffer*> m_UsedStagingBuffers[K_MAX_FRAMES_IN_FLIGHT];
		StagingBuffer* m_CurrentStagingBuffer;
		uint32 m_CurrentFrameIndex;

		static constexpr uint32 K_DEFAULT_STAGING_BUFFER_SIZE = 2 * 1024 * 1024;
		static constexpr uint32 K_STAGING_BUFFER_ALIGNMENT = 4;
	};
}

#include "graphicshal/vulkan/vulkanuploadmanager.inl"

#endif // KH_GFXAPI_VULKAN