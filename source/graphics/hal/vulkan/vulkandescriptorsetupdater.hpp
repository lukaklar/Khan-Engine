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
		void SetSampledImage(uint32_t binding, VkImageView imageView);
		void SetStorageBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
		void SetUAStorageBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
		void SetStorageImage(uint32_t binding, VkImageView imageView);
		void SetUAStorageImage(uint32_t binding, VkImageView imageView);
		void SetUniformTexelBuffer(uint32_t binding, const VkBufferView& bufferView);
		void SetStorageTexelBuffer(uint32_t binding, const VkBufferView& bufferView);
		void SetSampler(uint32_t binding, VkSampler sampler);

	private:
		static constexpr uint32_t K_MAX_BINDING_COUNT = 32 * ResourceBindFrequency_Count;
		static constexpr uint32_t K_HLSL_B_REGISTER_OFFSET = 0;
		static constexpr uint32_t K_HLSL_T_REGISTER_OFFSET = 4;
		static constexpr uint32_t K_HLSL_U_REGISTER_OFFSET = 12;
		static constexpr uint32_t K_HLSL_S_REGISTER_OFFSET = 20;

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