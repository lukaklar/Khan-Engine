#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "core/primitivetypes.h"
#include "graphicshal/config.h"

namespace Khan
{
	class VulkanCommandPool
	{
	public:
		void Create(VkDevice device, uint32 queueFamilyIndex);
		void Destroy();

		VkCommandBuffer AllocateCommandBuffer();
		void ResetFrame(uint32 frameIndex);

	private:
		VkDevice m_Device;
		struct FramePool
		{
			VkCommandPool m_CommandPool;
			TArray<VkCommandBuffer> m_CommandBuffers;
			uint32 m_Index;
		} m_FramePools[K_MAX_FRAMES_IN_FLIGHT], *m_CurrentPool;
	};
}

#include "graphicshal/vulkan/vulkancommandpool.inl"

#endif // KH_GFXAPI_VULKAN