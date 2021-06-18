#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/config.h"

namespace Khan
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice device);
		~VulkanDescriptorPool();

		void AllocateDescriptorSets(uint32_t descriptorSetCount, const VkDescriptorSetLayout* pSetLayouts, VkDescriptorSet* pDescriptorSet);
		void ResetFrame(uint32_t frameIndex);

	private:
		static constexpr uint32_t K_MAX_DESCRIPTOR_SETS = 100;

		static constexpr VkDescriptorPoolSize K_DESCRIPTOR_POOL_SIZES[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 16 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 32 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8 }
		};

		VkDevice m_Device;
		VkDescriptorPool m_CurrentPool;
		std::vector<VkDescriptorPool> m_FreePools;
		std::vector<VkDescriptorPool> m_UsedPools[K_MAX_FRAMES_IN_FLIGHT];
		uint32_t m_CurrentFrameIndex;
	};
}

#include "graphics/hal/vulkan/vulkandescriptorpool.inl"

#endif // KH_GFXAPI_VULKAN