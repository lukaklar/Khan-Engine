#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/resourcebindfrequency.hpp"

namespace Khan
{
	class VulkanDescriptorSetUpdater
	{
	public:
		VulkanDescriptorSetUpdater(VkDevice device);
		void Reset();
		void Update();
		void SetDescriptorSet(VkDescriptorSet descriptorSet);
		void SetUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
		void SetStorageBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
		void SetSampledImage(uint32_t binding, VkImageView imageView);
		void SetStorageImage(uint32_t binding, VkImageView imageView);
		void SetStorageTexelBuffer(uint32_t binding, const VkBufferView& bufferView);
		void SetSampler(uint32_t binding, VkSampler sampler);

	private:
		static constexpr uint32_t K_MAX_BINDING_COUNT = 32 * ResourceBindFrequency_Count;

		VkDevice m_Device;
		VkDescriptorSet m_DescriptorSet;
		VkWriteDescriptorSet m_Writes[K_MAX_BINDING_COUNT];
		uint32_t m_WriteCount;
		VkDescriptorImageInfo m_ImageInfos[K_MAX_BINDING_COUNT];
		uint32_t m_ImageCount;
		VkDescriptorBufferInfo m_BufferInfos[K_MAX_BINDING_COUNT];
		uint32_t m_BufferCount;
	};
}

#include "graphics/hal/vulkan/vulkandescriptorsetupdater.inl"

#endif // KH_GFXAPI_VULKAN