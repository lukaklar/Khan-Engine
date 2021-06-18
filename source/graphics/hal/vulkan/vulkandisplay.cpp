#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/config.h"
#include "graphics/hal/vulkan/vulkanbackend.hpp"
#include "graphics/hal/vulkan/vulkancore.h"
#include "graphics/hal/vulkan/vulkandisplay.hpp"
#include "graphics/hal/vulkan/vulkanadapter.hpp"
#include "system/window.hpp"

namespace Khan
{
	Display::Display(const DisplayAdapter& adapter)
	{
		VkWin32SurfaceCreateInfoKHR surfaceInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
		surfaceInfo.hinstance = GetModuleHandle(NULL);
		surfaceInfo.hwnd = Window::g_hWnd;

		VK_ASSERT(vkCreateWin32SurfaceKHR(RenderBackend::g_Instance, &surfaceInfo, nullptr, &m_Surface), "[VULKAN] Failed to create win32 surface.");

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter.VulkanAdapter(), m_Surface, &surfaceCapabilities), "[VULKAN] Failed to get physical device surface capabilities.");

		m_CurrentExtent = surfaceCapabilities.currentExtent;
		m_MinExtent = surfaceCapabilities.minImageExtent;
		m_MaxExtent = surfaceCapabilities.maxImageExtent;

		m_CompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (VkCompositeAlphaFlagBitsKHR compositeAlphaFlag : compositeAlphaFlags)
		{
			if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
			{
				m_CompositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		uint32_t presentModeCount;
		VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(adapter.VulkanAdapter(), m_Surface, &presentModeCount, nullptr), "[VULKAN] Failed to get present mode count.");
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(adapter.VulkanAdapter(), m_Surface, &presentModeCount, presentModes.data()), "[VULKAN] Failed to enumerate present modes.");

		for (uint32_t i = 0; i < presentModeCount; ++i)
		{
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_NoVSyncPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			else if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				m_NoVSyncPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		auto CalculateImageCount = [](uint32_t& imageCount, uint32_t minImageCount, uint32_t maxImageCount)
		{
			if (imageCount < minImageCount)
			{
				imageCount = minImageCount;
			}
			if (maxImageCount > 0 && imageCount > maxImageCount)
			{
				imageCount = maxImageCount;
			}
		};

		m_VSyncImageCount = K_MAX_FRAMES_IN_FLIGHT;
		CalculateImageCount(m_VSyncImageCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

		m_NoVSyncImageCount = K_MAX_FRAMES_IN_FLIGHT + 1;
		CalculateImageCount(m_NoVSyncImageCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			m_PreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			m_PreTransform = surfaceCapabilities.currentTransform;
		}

		uint32_t surfaceFormatCount;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.VulkanAdapter(), m_Surface, &surfaceFormatCount, nullptr), "[VULKAN] Failed to get surface format count.");
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(adapter.VulkanAdapter(), m_Surface, &surfaceFormatCount, surfaceFormats.data()), "[VULKAN] Failed to enumerate surface formats.");

		if ((surfaceFormatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_SDRSurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
			m_SDRSurfaceFormat.colorSpace = surfaceFormats[0].colorSpace;

			m_HDRSurfaceFormat.format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			m_HDRSurfaceFormat.colorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;
		}
		else
		{
			bool found_B8G8R8A8_UNORM = false;
			bool found_HDR10 = false;
			for (VkSurfaceFormatKHR surfaceFormat : surfaceFormats)
			{
				if (!found_B8G8R8A8_UNORM && surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					m_SDRSurfaceFormat.format = surfaceFormat.format;
					m_SDRSurfaceFormat.colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
				}
				else if (!found_HDR10 && surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 && surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
				{
					m_HDRSurfaceFormat.format = surfaceFormat.format;
					m_HDRSurfaceFormat.colorSpace = surfaceFormat.colorSpace;
					found_HDR10 = true;
				}
			}

			if (!found_B8G8R8A8_UNORM)
			{
				m_SDRSurfaceFormat.format = surfaceFormats[0].format;
				m_SDRSurfaceFormat.colorSpace = surfaceFormats[0].colorSpace;
			}

			if (!found_HDR10)
			{
				m_HDRSurfaceFormat = m_SDRSurfaceFormat;
			}
		}

		VkBool32 supportsPresent;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(adapter.VulkanAdapter(), adapter.GetQueueFamilyIndices()[QueueType_Graphics], m_Surface, &supportsPresent), "[VULKAN] Failed to query physical device surface support.");
	}

	Display::~Display()
	{
		vkDestroySurfaceKHR(RenderBackend::g_Instance, m_Surface, nullptr);
	}
}

#endif // KH_GFXAPI_VULKAN