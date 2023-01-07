#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/config.h"
#include "graphicshal/swapchain.hpp"

namespace Khan
{
	class Display;
	class RenderDevice;
	class VulkanRenderDevice;

	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain(RenderDevice& device, Display& display);
		virtual ~VulkanSwapchain() override;

		virtual void Flip() override;

	private:
		void CreateSwapchain(uint32_t width, uint32_t height, bool vsync, bool hdr);
		void CreateSynchronizationPrimitives();

		VulkanRenderDevice& m_Device;
		Display& m_Display;
		VkSwapchainKHR m_Swapchain;
		VkSemaphore m_RenderCompleteSemaphores[K_MAX_FRAMES_IN_FLIGHT];
		VkSemaphore m_PresentCompleteSemaphores[K_MAX_FRAMES_IN_FLIGHT];
		VkFence m_FrameFences[K_MAX_FRAMES_IN_FLIGHT];
	};
}

#endif // KH_GFXAPI_VULKAN