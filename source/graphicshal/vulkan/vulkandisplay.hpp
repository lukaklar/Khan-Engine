#pragma once

#ifdef KH_GFXAPI_VULKAN

namespace Khan
{
	class DisplayAdapter;

	class Display
	{
	public:
		Display(const DisplayAdapter& adapter);
		~Display();

		inline VkSurfaceKHR VulkanSurface() const { return m_Surface; }
		inline VkExtent2D VulkanCurrentExtent() const { return m_CurrentExtent; }
		inline VkExtent2D VulkanMinExtent() const { return m_MinExtent; }
		inline VkExtent2D VulkanMaxExtent() const { return m_MaxExtent; }
		inline VkCompositeAlphaFlagBitsKHR VulkanCompositeAlpha() const { return m_CompositeAlpha; }
		inline VkPresentModeKHR VulkanNoVSyncPresentMode() const { return m_NoVSyncPresentMode; }
		inline uint32_t VulkanVSyncImageCount() const { return m_VSyncImageCount; }
		inline uint32_t VulkanNoVSyncImageCount() const { return m_NoVSyncImageCount; }
		inline VkSurfaceTransformFlagBitsKHR VulkanPreTransform() const { return m_PreTransform; }
		inline VkSurfaceFormatKHR VulkanHDRSurfaceFormat() const { return m_HDRSurfaceFormat; }
		inline VkSurfaceFormatKHR VulkanSDRSurfaceFormat() const { return m_SDRSurfaceFormat; }

	private:
		VkSurfaceKHR m_Surface;
		VkExtent2D m_CurrentExtent;
		VkExtent2D m_MinExtent;
		VkExtent2D m_MaxExtent;
		VkCompositeAlphaFlagBitsKHR m_CompositeAlpha;
		VkPresentModeKHR m_NoVSyncPresentMode;
		uint32_t m_VSyncImageCount;
		uint32_t m_NoVSyncImageCount;
		VkSurfaceTransformFlagBitsKHR m_PreTransform;
		VkSurfaceFormatKHR m_HDRSurfaceFormat;
		VkSurfaceFormatKHR m_SDRSurfaceFormat;
	};
}

#endif // KH_GFXAPI_VULKAN