#pragma once

#ifdef KH_GFXAPI_VULKAN

// TODO: Move this to core somewhere
#define KH_ALIGN(num, alignment) ((num + alignment - 1) & ~(alignment - 1))

namespace Khan
{
	KH_FORCE_INLINE uint32_t VulkanUploadManager::Upload(const void* src, uint32_t size)
	{
		m_Lock.lock();

		if (m_CurrentStagingBuffer == nullptr || KH_ALIGN(m_CurrentStagingBuffer->m_Offset, K_STAGING_BUFFER_ALIGNMENT) + size >= m_CurrentStagingBuffer->m_Capacity)
		{
			StagingBuffer* oldStagingBuffer = m_CurrentStagingBuffer;

			m_CurrentStagingBuffer = nullptr;
			for (auto& it = m_FreeStagingBuffers.begin(); it != m_FreeStagingBuffers.end(); ++it)
			{
				StagingBuffer* stagingBuffer = *it;
				if (stagingBuffer->m_Capacity >= size)
				{
					m_CurrentStagingBuffer = stagingBuffer;
					m_FreeStagingBuffers.erase(it);
					break;
				}
			}

			if (m_CurrentStagingBuffer == nullptr)
			{
				m_CurrentStagingBuffer = new StagingBuffer();
				m_CurrentStagingBuffer->m_Capacity = size > K_DEFAULT_STAGING_BUFFER_SIZE ? size : K_DEFAULT_STAGING_BUFFER_SIZE;
				m_CurrentStagingBuffer->m_Offset = 0;

				VkBufferCreateInfo info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
				info.size = m_CurrentStagingBuffer->m_Capacity;
				info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

				VmaAllocationCreateInfo allocInfo = {};
				allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

				VK_ASSERT(vmaCreateBuffer(m_Allocator, &info, &allocInfo, &m_CurrentStagingBuffer->m_Buffer, &m_CurrentStagingBuffer->m_Allocation, nullptr), "[VULKAN] Failed to allocate a new staging buffer.");
				VK_ASSERT(vmaMapMemory(m_Allocator, m_CurrentStagingBuffer->m_Allocation, reinterpret_cast<void**>(&m_CurrentStagingBuffer->m_MappedMemory)), "[VULKAN] Failed to map staging buffer memory.");
			}

			if (oldStagingBuffer != nullptr)
			{
				m_UsedStagingBuffers[m_CurrentFrameIndex].push_back(oldStagingBuffer);
			}
		}

		uint32_t startOffset = KH_ALIGN(m_CurrentStagingBuffer->m_Offset, K_STAGING_BUFFER_ALIGNMENT);
		m_CurrentStagingBuffer->m_Offset = startOffset + size;

		m_Lock.unlock();

		std::memcpy(m_CurrentStagingBuffer->m_MappedMemory + startOffset, src, size);
	}

	KH_FORCE_INLINE void VulkanUploadManager::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;
		m_CurrentStagingBuffer = nullptr;
		for (StagingBuffer* stagingBuffer : m_UsedStagingBuffers[frameIndex])
		{
			stagingBuffer->m_Offset = 0;
			m_FreeStagingBuffers.push_back(stagingBuffer);
		}
		m_UsedStagingBuffers[frameIndex].clear();
	}

	KH_FORCE_INLINE VkBuffer VulkanUploadManager::CurrentBuffer() const
	{
		return m_CurrentStagingBuffer->m_Buffer;
	}
}

#endif // KH_GFXAPI_VULKAN