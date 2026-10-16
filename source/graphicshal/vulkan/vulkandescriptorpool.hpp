#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "core/primitivetypes.h"
#include "graphicshal/config.h"

namespace Khan
{
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VkDevice device);
		~VulkanDescriptorPool();

		void AllocateDescriptorSets(uint32 descriptorSetCount, const VkDescriptorSetLayout* pSetLayouts, VkDescriptorSet* pDescriptorSet);
		void ResetFrame(uint32 frameIndex);

	private:
		static constexpr uint32 K_MAX_DESCRIPTOR_SETS = 100;

		static constexpr VkDescriptorPoolSize K_DESCRIPTOR_POOL_SIZES[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 16 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 32 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8 }
		};

		VkDevice m_Device;
		VkDescriptorPool m_CurrentPool;
		TArray<VkDescriptorPool> m_FreePools;
		TArray<VkDescriptorPool> m_UsedPools[K_MAX_FRAMES_IN_FLIGHT];
		uint32 m_CurrentFrameIndex;
	};
}

#include "graphicshal/vulkan/vulkandescriptorpool.inl"

#endif // KH_GFXAPI_VULKAN