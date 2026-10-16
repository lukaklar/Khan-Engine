#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/primitivetypes.h"
#include "graphicshal/config.h"
#include "graphicshal/vulkan/vk_mem_alloc.h"

namespace Khan
{
	class VulkanUniformBufferAllocator
	{
	public:
		void Create(VmaAllocator allocator);
		void Destroy(VmaAllocator allocator);

		uint32 Upload(const void* src, uint32 size);
		void ResetFrame(uint32 frameIndex);

		VkBuffer CurrentBuffer() const;

	private:
		static constexpr uint32 K_UNIFORM_BUFFER_SIZE = 16 * 1024 * 1024;

		struct FramePool
		{
			void Create(VmaAllocator allocator);
			void Destroy(VmaAllocator allocator);

			VkBuffer m_Buffer;
			VmaAllocation m_Allocation;
			byte* m_MappedMemory;
			std::atomic<uint32> m_Offset;
		} m_FramePools[K_MAX_FRAMES_IN_FLIGHT], *m_CurrentPool;
	};
}

#include "graphicshal/vulkan/vulkanuniformbufferallocator.inl"

#endif // KH_GFXAPI_VULKAN