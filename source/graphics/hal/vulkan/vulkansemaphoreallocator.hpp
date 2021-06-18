#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/config.h"

namespace Khan
{
	class VulkanSemaphoreAllocator
	{
	public:
		void Create(VkDevice device);
		void Destroy();

		VkSemaphore AllocateSemaphore();
		void ResetFrame(uint32_t frameIndex);

	private:
		VkDevice m_Device;
		std::vector<VkSemaphore> m_FreeSemaphores;
		std::vector<VkSemaphore> m_UsedSemaphores[K_MAX_FRAMES_IN_FLIGHT];
		uint32_t m_CurrentFrameIndex;
	};
}

#include "graphics/hal/vulkan/vulkansemaphoreallocator.inl"

#endif // KH_GFXAPI_VULKAN