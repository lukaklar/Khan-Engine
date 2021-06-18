#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanpipelinestatemanager.hpp"

namespace Khan
{
	void VulkanPipelineStateManager::Create(VkDevice device)
	{
		// TODO: Load pipeline cache
		m_PipelineCache = VK_NULL_HANDLE;
	}

	void VulkanPipelineStateManager::Destroy(VkDevice device)
	{
		// TODO: Save and destroy pipeline cache

		for (auto& it : m_GraphicsPipelineCache)
		{
			VulkanPipelineState& pipelineState = *it.second;

			vkDestroyPipeline(device, pipelineState.m_Pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelineState.m_PipelineLayout, nullptr);
			for (VkDescriptorSetLayout descriptorSetLayout : pipelineState.m_DescriptorSetLayout)
			{
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			}
		}

		for (auto& it : m_ComputePipelineCache)
		{
			VulkanPipelineState& pipelineState = *it.second;

			vkDestroyPipeline(device, pipelineState.m_Pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelineState.m_PipelineLayout, nullptr);
			for (VkDescriptorSetLayout descriptorSetLayout : pipelineState.m_DescriptorSetLayout)
			{
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			}
		}
	}
}

#endif // KH_GFXAPI_VULKAN