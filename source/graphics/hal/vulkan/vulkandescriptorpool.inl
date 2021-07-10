#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkancore.h"

namespace Khan
{
	KH_FORCE_INLINE void VulkanDescriptorPool::AllocateDescriptorSets(uint32_t descriptorSetCount, const VkDescriptorSetLayout* pSetLayouts, VkDescriptorSet* pDescriptorSet)
	{
		VkDescriptorSetAllocateInfo allocateInfo;
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.descriptorPool = m_CurrentPool;
		allocateInfo.descriptorSetCount = descriptorSetCount;
		allocateInfo.pSetLayouts = pSetLayouts;

		if (m_CurrentPool == VK_NULL_HANDLE || vkAllocateDescriptorSets(m_Device, &allocateInfo, pDescriptorSet) == VK_ERROR_OUT_OF_POOL_MEMORY)
		{
			if (m_FreePools.empty())
			{
				VkDescriptorPoolCreateInfo info;
				info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				info.pNext = nullptr;
				info.flags = 0;
				info.maxSets = K_MAX_DESCRIPTOR_SETS;
				info.poolSizeCount = _countof(K_DESCRIPTOR_POOL_SIZES);
				info.pPoolSizes = K_DESCRIPTOR_POOL_SIZES;

				VK_ASSERT(vkCreateDescriptorPool(m_Device, &info, nullptr, &m_CurrentPool), "[VULKAN] Failed to create descriptor pool.");
			}
			else
			{
				m_CurrentPool = m_FreePools.back();
				m_FreePools.pop_back();
			}

			m_UsedPools[m_CurrentFrameIndex].push_back(m_CurrentPool);
			allocateInfo.descriptorPool = m_CurrentPool;
			VK_ASSERT(vkAllocateDescriptorSets(m_Device, &allocateInfo, pDescriptorSet), "[VULKAN] Failed to allocate descriptor sets.");
		}
	}

	KH_FORCE_INLINE void VulkanDescriptorPool::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentFrameIndex = frameIndex;
		std::vector<VkDescriptorPool>& framePool = m_UsedPools[frameIndex];
		for (VkDescriptorPool pool : framePool)
		{
			VK_ASSERT(vkResetDescriptorPool(m_Device, pool, 0), "[VULKAN] Failed to reset descriptor pool.");
		}
		m_FreePools.insert(m_FreePools.end(), framePool.begin(), framePool.end());
		framePool.clear();
		m_CurrentPool = VK_NULL_HANDLE;
	}
}

#endif // KH_GFXAPI_VULKAN