#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/config.h"
#include "graphics/hal/vulkan/vulkantexture.hpp"

namespace Khan
{
	class Display;
	class RenderDevice;

	class Swapchain
	{
	public:
		Swapchain(RenderDevice& device, Display& display);
		~Swapchain();

		void Flip();
		inline Texture* GetCurrentBackBuffer() const { return m_BackBuffers[m_CurrentImageIndex]; }

	private:
		void CreateSwapchain(uint32_t width, uint32_t height, bool vsync, bool hdr);
		void CreateSynchronizationPrimitives();

		RenderDevice& m_Device;
		Display& m_Display;
		VkSwapchainKHR m_Swapchain;
		std::vector<VulkanTexture*> m_BackBuffers;
		VkSemaphore m_RenderCompleteSemaphores[K_MAX_FRAMES_IN_FLIGHT];
		VkSemaphore m_PresentCompleteSemaphores[K_MAX_FRAMES_IN_FLIGHT];
		VkFence m_FrameFences[K_MAX_FRAMES_IN_FLIGHT];
		uint32_t m_CurrentImageIndex;
		uint32_t m_FrameIndex;
	};
}

#endif // KH_GFXAPI_VULKAN