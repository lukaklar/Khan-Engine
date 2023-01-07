#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkancore.h"
#include "graphicshal/vulkan/vulkanuploadmanager.hpp"

namespace Khan
{
	void VulkanUploadManager::Create(VmaAllocator allocator)
	{
		m_Allocator = allocator;
		m_CurrentStagingBuffer = nullptr;
		m_CurrentFrameIndex = 0;
	}

	void VulkanUploadManager::Destroy()
	{
		for (uint32_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; ++i)
		{
			m_FreeStagingBuffers.insert(m_FreeStagingBuffers.end(), m_UsedStagingBuffers[i].begin(), m_UsedStagingBuffers[i].end());
		}
		for (StagingBuffer* stagingBuffer : m_FreeStagingBuffers)
		{
			vmaUnmapMemory(m_Allocator, stagingBuffer->m_Allocation);
			vmaDestroyBuffer(m_Allocator, stagingBuffer->m_Buffer, stagingBuffer->m_Allocation);
			delete stagingBuffer;
		}
	}
}

#endif // KH_GFXAPI_VULKAN