#pragma once

#ifdef KH_GFXAPI_VULKAN

namespace Khan
{
    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::Reset()
    {
        m_WriteCount = 0;
        m_BufferCount = 0;
        m_ImageCount = 0;
        m_DescriptorSet = VK_NULL_HANDLE;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::Update()
    {
        vkUpdateDescriptorSets(m_Device, m_WriteCount, m_Writes, 0, nullptr);
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetDescriptorSet(VkDescriptorSet descriptorSet)
    {
        m_DescriptorSet = descriptorSet;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
    {
        VkWriteDescriptorSet& write = m_Writes[m_WriteCount++];
        write.dstSet = m_DescriptorSet;
        write.dstBinding = binding;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        VkDescriptorBufferInfo& info = m_BufferInfos[m_BufferCount++];
        write.pBufferInfo = &info;
        info.buffer = buffer;
        info.offset = offset;
        info.range = range;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetStorageBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
    {
        VkWriteDescriptorSet& write = m_Writes[m_WriteCount++];
        write.dstSet = m_DescriptorSet;
        write.dstBinding = binding;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        VkDescriptorBufferInfo& info = m_BufferInfos[m_BufferCount++];
        write.pBufferInfo = &info;
        info.buffer = buffer;
        info.offset = offset;
        info.range = range;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetSampledImage(uint32_t binding, VkImageView imageView)
    {
        VkWriteDescriptorSet& write = m_Writes[m_WriteCount++];
        write.dstSet = m_DescriptorSet;
        write.dstBinding = binding;
        write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        VkDescriptorImageInfo& info = m_ImageInfos[m_ImageCount++];
        write.pImageInfo = &info;
        info.imageView = imageView;
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetStorageImage(uint32_t binding, VkImageView imageView)
    {
        VkWriteDescriptorSet& write = m_Writes[m_WriteCount++];
        write.dstSet = m_DescriptorSet;
        write.dstBinding = binding;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        VkDescriptorImageInfo& info = m_ImageInfos[m_ImageCount++];
        write.pImageInfo = &info;
        info.imageView = imageView;
        info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetStorageTexelBuffer(uint32_t binding, const VkBufferView& bufferView)
    {
        VkWriteDescriptorSet& write = m_Writes[m_WriteCount++];
        write.dstSet = m_DescriptorSet;
        write.dstBinding = binding;
        write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        VkDescriptorBufferInfo& info = m_BufferInfos[m_BufferCount++];
        write.pTexelBufferView = &bufferView;
    }

    KH_FORCE_INLINE void VulkanDescriptorSetUpdater::SetSampler(uint32_t binding, VkSampler sampler)
    {
        VkWriteDescriptorSet& write = m_Writes[m_WriteCount++];
        write.dstSet = m_DescriptorSet;
        write.dstBinding = binding;
        write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        VkDescriptorImageInfo& info = m_ImageInfos[m_ImageCount++];
        write.pImageInfo = &info;
        info.sampler = sampler;
    }
}

#endif // KH_GFXAPI_VULKAN