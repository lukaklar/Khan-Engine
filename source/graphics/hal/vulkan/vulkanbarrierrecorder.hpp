#pragma once

#ifdef KH_GFXAPI_VULKAN

namespace Khan
{
	enum QueueType;
	enum ResourceState;
	class DisplayAdapter;
	class VulkanBuffer;
	class VulkanBufferView;
	class VulkanTexture;
	class VulkanTextureView;

	class VulkanBarrierRecorder
	{
	public:
		VulkanBarrierRecorder(const DisplayAdapter& adapter);

		void RecordBarrier(VulkanBuffer& buffer, ResourceState state, QueueType queue);
		void RecordBarrier(VulkanBufferView& bufferView, ResourceState state, QueueType queue);
		void RecordBarrier(VulkanTexture& texture, ResourceState state, QueueType queue);
		void RecordBarrier(VulkanTextureView& textureView, ResourceState state, QueueType queue);
		void Flush(VkCommandBuffer commandBuffer);
		void Reset();

	private:
		const DisplayAdapter& m_Adapter;
		VkImageMemoryBarrier m_ImageBarriers[32];
		uint32_t m_ImageBarrierCount;
		VkBufferMemoryBarrier m_BufferBarriers[32];
		uint32_t m_BufferBarrierCount;
		VkPipelineStageFlags m_SrcStages;
		VkPipelineStageFlags m_DstStages;
	};
}

#include "graphics/hal/vulkan/vulkanbarrierrecorder.inl"

#endif // KH_GFXAPI_VULKAN