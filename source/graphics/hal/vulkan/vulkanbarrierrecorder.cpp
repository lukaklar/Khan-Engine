#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanbarrierrecorder.hpp"

namespace Khan
{
	VulkanBarrierRecorder::VulkanBarrierRecorder(const DisplayAdapter& adapter)
		: m_Adapter(adapter)
	{
		for (uint32_t i = 0; i < 32; ++i)
		{
			VkImageMemoryBarrier& imageBarrier = m_ImageBarriers[i];
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier.pNext = nullptr;

			VkBufferMemoryBarrier& bufferBarrier = m_BufferBarriers[i];
			bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferBarrier.pNext = nullptr;

			VkMemoryBarrier& memoryBarrier = m_MemoryBarriers[i];
			memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			memoryBarrier.pNext = nullptr;
		}

		Reset();
	}
}

#endif // KH_GFXAPI_VULKAN