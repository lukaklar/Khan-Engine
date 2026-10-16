#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "core/primitivetypes.h"
#include "graphicshal/config.h"

namespace Khan
{
	class VulkanSemaphoreAllocator
	{
	public:
		void Create(VkDevice device);
		void Destroy();

		VkSemaphore AllocateSemaphore();
		void ResetFrame(uint32 frameIndex);

	private:
		VkDevice m_Device;
		TArray<VkSemaphore> m_FreeSemaphores;
		TArray<VkSemaphore> m_UsedSemaphores[K_MAX_FRAMES_IN_FLIGHT];
		uint32 m_CurrentFrameIndex;
	};
}

#include "graphicshal/vulkan/vulkansemaphoreallocator.inl"

#endif // KH_GFXAPI_VULKAN