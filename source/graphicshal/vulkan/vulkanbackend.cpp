#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/renderbackend.hpp"
#include "graphicshal/vulkan/vulkanadapter.hpp"
#include "graphicshal/vulkan/vulkancore.h"
#include "graphicshal/vulkan/vulkandevice.hpp"
#include "graphicshal/vulkan/vulkandisplay.hpp"
#include "graphicshal/vulkan/vulkanswapchain.hpp"
#include "graphicshal/vulkan/vulkanutils.hpp"

namespace Khan
{
	namespace RenderBackend
	{
		VkInstance g_Instance;

#ifdef VK_VALIDATE
		static bool s_ValidationLayersEnabled;
		static VkDebugUtilsMessengerEXT s_DebugMessenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {

			if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
				OutputDebugString(pCallbackData->pMessage);
				OutputDebugString("\n");
			}

			return VK_FALSE;
		}
#endif // VK_VALIDATE

		inline static void CreateInstance(bool validationEnabled)
		{
			std::vector<const char*> enabledLayers;

#ifdef VK_VALIDATE
			s_ValidationLayersEnabled = validationEnabled;
			if (s_ValidationLayersEnabled)
			{
				uint32_t layerPropertyCount;
				VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr), "[VULKAN] Failed to get instance layer property count.");
				std::vector<VkLayerProperties> layerProperties(layerPropertyCount);
				VK_ASSERT(vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data()), "[VULKAN] Failed to enumerate instance layer properties.");

				const char* debugLayerName = "VK_LAYER_KHRONOS_validation";
				if (CheckLayer(debugLayerName, layerProperties))
				{
					enabledLayers.push_back(debugLayerName);
				}
			}
#endif // VK_VALIDATE

			uint32_t extensionPropertyCount;
			VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr), "[VULKAN] Failed to get instance extension property count.");
			std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
			VK_ASSERT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensionProperties.data()), "[VULKAN] Failed to enumerate instance extension properties.");

			std::vector<const char*> requiredExtensions =
			{
				VK_KHR_SURFACE_EXTENSION_NAME,
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME
			};

			for (const char* extension : requiredExtensions)
			{
				if (!CheckExtension(extension, extensionProperties))
				{
					KH_ASSERT(false, "[VULKAN] Not all extensions are supported.\n");
				}
			}

#ifdef VK_VALIDATE
			const char* debugExtensionName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
			if (s_ValidationLayersEnabled && CheckExtension(debugExtensionName, extensionProperties))
			{
				requiredExtensions.push_back(debugExtensionName);
			}
#endif // VK_VALIDATE

			VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
			appInfo.pApplicationName = "Khan Engine";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Khan Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_1;

			VkInstanceCreateInfo instanceInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
			instanceInfo.ppEnabledLayerNames = enabledLayers.data();
			instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
			instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();

#ifdef VK_VALIDATE
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
			debugUtilsMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerInfo.pfnUserCallback = DebugCallback;
			debugUtilsMessengerInfo.pUserData = nullptr;

			if (s_ValidationLayersEnabled)
			{
				instanceInfo.pNext = &debugUtilsMessengerInfo;
			}
#endif // VK_VALIDATE

			VK_ASSERT(vkCreateInstance(&instanceInfo, nullptr, &g_Instance), "[VULKAN] Failed to create instance.");

#ifdef VK_VALIDATE
			if (s_ValidationLayersEnabled)
			{
				PFN_vkCreateDebugUtilsMessengerEXT fpvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(g_Instance, "vkCreateDebugUtilsMessengerEXT"));
				if (fpvkCreateDebugUtilsMessengerEXT != nullptr)
				{
					VK_ASSERT(fpvkCreateDebugUtilsMessengerEXT(g_Instance, &debugUtilsMessengerInfo, nullptr, &s_DebugMessenger), "[VULKAN] Failed to create debug utils messenger.");
				}
			}
#endif // VK_VALIDATE
		}

		inline static void EnumerateAdapters()
		{
			uint32_t physicalDeviceCount;
			VK_ASSERT(vkEnumeratePhysicalDevices(g_Instance, &physicalDeviceCount, nullptr), "[VULKAN] Failed to get physical device count.");
			std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
			VK_ASSERT(vkEnumeratePhysicalDevices(g_Instance, &physicalDeviceCount, physicalDevices.data()), "[VULKAN] Failed to enumerate physical devices.");

			g_Adapters.reserve(physicalDeviceCount);
			for (VkPhysicalDevice physicalDevice : physicalDevices)
			{
				g_Adapters.push_back(new DisplayAdapter(physicalDevice));
			}
		}

		void Initialize(bool enableDebugMode)
		{
			CreateInstance(enableDebugMode);
			EnumerateAdapters();

			g_Device = new VulkanRenderDevice(*g_Adapters[0]);
			g_Display = new Display(g_Device->GetAdapter());
			g_Swapchain = new VulkanSwapchain(*g_Device, *g_Display);
		}

		inline static void DestroyInstance()
		{
#ifdef VK_VALIDATE
			if (s_ValidationLayersEnabled)
			{
				PFN_vkDestroyDebugUtilsMessengerEXT fpvkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugUtilsMessengerEXT"));
				if (fpvkDestroyDebugUtilsMessengerEXT != nullptr)
				{
					fpvkDestroyDebugUtilsMessengerEXT(g_Instance, s_DebugMessenger, nullptr);
				}
			}
#endif // VK_VALIDATE
			vkDestroyInstance(g_Instance, nullptr);
		}

		void Shutdown()
		{
			g_Device->WaitIdle();

			delete g_Swapchain;
			delete g_Display;
			delete g_Device;

			for (DisplayAdapter* adapter : g_Adapters)
			{
				delete adapter;
			}

			DestroyInstance();
		}
	}
}

#endif // KH_GFXAPI_VULKAN