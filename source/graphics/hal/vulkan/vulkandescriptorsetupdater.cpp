#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkandescriptorsetupdater.hpp"

namespace Khan
{
    VulkanDescriptorSetUpdater::VulkanDescriptorSetUpdater(VkDevice device)
		: m_Device(device)
	{
		for (uint32_t i = 0; i < K_MAX_BINDING_COUNT; ++i)
		{
			VkWriteDescriptorSet& write = m_Writes[i];
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.pNext = nullptr;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
		}
	}
}

#endif // KH_GFXAPI_VULKAN