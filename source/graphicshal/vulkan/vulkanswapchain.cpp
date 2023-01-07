#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/pixelformats.hpp"
#include "graphicshal/vulkan/vulkanswapchain.hpp"
#include "graphicshal/vulkan/vulkandevice.hpp"
#include "graphicshal/vulkan/vulkandisplay.hpp"
#include "core/defines.h"

namespace Khan
{
	VulkanSwapchain::VulkanSwapchain(RenderDevice& device, Display& display)
		: m_Swapchain(VK_NULL_HANDLE)
		, m_Device(reinterpret_cast<VulkanRenderDevice&>(device))
		, m_Display(display)
	{
		// TODO: Remove these hardcoded values
		CreateSwapchain(1280, 720, false, false);
		CreateSynchronizationPrimitives();

		VK_ASSERT(vkAcquireNextImageKHR(m_Device.VulkanDevice(), m_Swapchain, UINT64_MAX, m_PresentCompleteSemaphores[m_FrameIndex], VK_NULL_HANDLE, &m_CurrentImageIndex), "Failed to acquire next image.");
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		for (uint32_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroyFence(m_Device.VulkanDevice(), m_FrameFences[i], nullptr);
			vkDestroySemaphore(m_Device.VulkanDevice(), m_PresentCompleteSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device.VulkanDevice(), m_RenderCompleteSemaphores[i], nullptr);
		}

		for (Texture* texture : m_BackBuffers)
		{
			delete reinterpret_cast<VulkanTexture*>(texture);
		}

		vkDestroySwapchainKHR(m_Device.VulkanDevice(), m_Swapchain, nullptr);
	}

	void VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height, bool vsync, bool hdr)
	{
		VkSwapchainKHR oldSwapchain = m_Swapchain;

		VkExtent2D swapchainImageExtent;
		if (m_Display.VulkanCurrentExtent().width == UINT32_MAX)
		{
			swapchainImageExtent.width = std::max(m_Display.VulkanMinExtent().width, std::min(m_Display.VulkanMaxExtent().width, width));
			swapchainImageExtent.height = std::max(m_Display.VulkanMinExtent().height, std::min(m_Display.VulkanMaxExtent().height, height));
		}
		else
		{
			swapchainImageExtent = m_Display.VulkanCurrentExtent();
		}

		VkSurfaceFormatKHR& surfaceFormat = hdr ? m_Display.VulkanHDRSurfaceFormat() : m_Display.VulkanSDRSurfaceFormat();

		VkSwapchainCreateInfoKHR swapchainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchainInfo.surface = m_Display.VulkanSurface();
		swapchainInfo.minImageCount = vsync ? m_Display.VulkanVSyncImageCount() : m_Display.VulkanNoVSyncImageCount();
		swapchainInfo.imageFormat = surfaceFormat.format;
		swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainInfo.imageExtent = swapchainImageExtent;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // TODO: Maybe add transfer src in order to take screenshots and definitely add sampled bit for posteffects
		swapchainInfo.preTransform = m_Display.VulkanPreTransform();
		swapchainInfo.compositeAlpha = m_Display.VulkanCompositeAlpha();
		swapchainInfo.presentMode = vsync ? VK_PRESENT_MODE_FIFO_KHR : m_Display.VulkanNoVSyncPresentMode();
		swapchainInfo.clipped = VK_TRUE;
		swapchainInfo.oldSwapchain = oldSwapchain;

		VK_ASSERT(vkCreateSwapchainKHR(m_Device.VulkanDevice(), &swapchainInfo, nullptr, &m_Swapchain), "[VULKAN] Failed to create swapchain.");

		// TODO: This can be removed but it should be profiled when recreating the swapchain if it is faster with or without this check
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_Device.VulkanDevice(), oldSwapchain, nullptr);
		}

		for (Texture* texture : m_BackBuffers)
		{
			delete reinterpret_cast<VulkanTexture*>(texture);
		}

		uint32_t swapchainImageCount;
		VK_ASSERT(vkGetSwapchainImagesKHR(m_Device.VulkanDevice(), m_Swapchain, &swapchainImageCount, nullptr), "[VULKAN] Failed to get swapchain image count.");
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		VK_ASSERT(vkGetSwapchainImagesKHR(m_Device.VulkanDevice(), m_Swapchain, &swapchainImageCount, swapchainImages.data()), "[VULKAN] Failed to get swapchain images.");

		TextureDesc textureDesc;
		textureDesc.m_Type = TextureType_2D;
		textureDesc.m_Width = swapchainImageExtent.width;
		textureDesc.m_Height = swapchainImageExtent.height;
		textureDesc.m_Depth = 1;
		textureDesc.m_ArrayLayers = 1;
		textureDesc.m_MipLevels = 1;
		textureDesc.m_Format = hdr ? PF_R10G10B10A2_UNORM : PF_B8G8R8A8_UNORM;
		textureDesc.m_Flags = TextureFlag_AllowRenderTarget | TextureFlag_AllowShaderResource | TextureFlag_AllowUnorderedAccess | TextureFlag_Writable | TextureFlag_Readable;

		m_BackBuffers.resize(swapchainImageCount);
		for (uint32_t i = 0; i < swapchainImageCount; ++i)
		{
			m_BackBuffers[i] = new VulkanTexture(swapchainImages[i], nullptr, textureDesc);
			KH_DEBUGONLY(m_BackBuffers[i]->SetDebugName("SwapChainBackBuffer");)
		}
	}

	inline void VulkanSwapchain::CreateSynchronizationPrimitives()
	{
		VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < K_MAX_FRAMES_IN_FLIGHT; ++i)
		{
			VK_ASSERT(vkCreateSemaphore(m_Device.VulkanDevice(), &semaphoreInfo, nullptr, &m_RenderCompleteSemaphores[i]), "[VULKAN] Failed to create render complete semaphore.");
			VK_ASSERT(vkCreateSemaphore(m_Device.VulkanDevice(), &semaphoreInfo, nullptr, &m_PresentCompleteSemaphores[i]), "[VULKAN] Failed to create present complete semaphore.");
			VK_ASSERT(vkCreateFence(m_Device.VulkanDevice(), &fenceInfo, nullptr, &m_FrameFences[i]), "[VULKAN] Failed to create frame fence.");
		}
	}

	void VulkanSwapchain::Flip()
	{
		m_Device.FlushCommands();

		VK_ASSERT(vkResetFences(m_Device.VulkanDevice(), 1, &m_FrameFences[m_FrameIndex]), "[VULKAN] Failed to reset frame fence.");

		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_PresentCompleteSemaphores[m_FrameIndex];
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;
		//submitInfo.commandBufferCount = 0;
		//submitInfo.pCommandBuffers = nullptr;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderCompleteSemaphores[m_FrameIndex];

		VK_ASSERT(vkQueueSubmit(m_Device.m_CommandQueues[QueueType_Graphics], 1, &submitInfo, m_FrameFences[m_FrameIndex]), "[VULKAN] Failed to submit work to graphics queue.");

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderCompleteSemaphores[m_FrameIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &m_CurrentImageIndex;

		VK_ASSERT(vkQueuePresentKHR(m_Device.m_CommandQueues[QueueType_Graphics], &presentInfo), "[VULKAN] Failed to present swapchain image to graphics queue");

		m_FrameIndex = (m_FrameIndex + 1) % K_MAX_FRAMES_IN_FLIGHT;

		VK_ASSERT(vkWaitForFences(m_Device.VulkanDevice(), 1, &m_FrameFences[m_FrameIndex], VK_TRUE, UINT64_MAX), "[VULKAN] Failed to wait for frame fence.");

		m_Device.OnFlip(m_FrameIndex);

		VK_ASSERT(vkAcquireNextImageKHR(m_Device.VulkanDevice(), m_Swapchain, UINT64_MAX, m_PresentCompleteSemaphores[m_FrameIndex], VK_NULL_HANDLE, &m_CurrentImageIndex), "Failed to acquire next image.");
	}
}

#endif // KH_GFXAPI_VULKAN