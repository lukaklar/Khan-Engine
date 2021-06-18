#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/config.h"

namespace Khan
{
	class VulkanCommandPool
	{
	public:
		void Create(VkDevice device, uint32_t queueFamilyIndex);
		void Destroy();

		VkCommandBuffer AllocateCommandBuffer();
		void ResetFrame(uint32_t frameIndex);

	private:
		VkDevice m_Device;
		struct FramePool
		{
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
			uint32_t m_Index;
		} m_FramePools[K_MAX_FRAMES_IN_FLIGHT], *m_CurrentPool;;
	};
}

#include "graphics/hal/vulkan/vulkancommandpool.inl"

#endif // KH_GFXAPI_VULKAN