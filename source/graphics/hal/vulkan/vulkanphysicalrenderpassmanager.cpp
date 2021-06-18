#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanphysicalrenderpassmanager.hpp"

namespace Khan
{
	void VulkanPhysicalRenderPassManager::Destroy(VkDevice device)
	{
		for (auto& it : m_PhysicalRenderPassCache)
		{
			vkDestroyRenderPass(device, it.second->VulkanRenderPass(), nullptr);
		}
	}
}

#endif // KH_GFXAPI_VULKAN