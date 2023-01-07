#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/pipelinedescriptions.hpp"
#include "graphicshal/resourcebindfrequency.hpp"

namespace Khan
{
	struct VulkanPipelineState : RenderPipelineState
	{
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout[ResourceBindFrequency_Count];
	};

	class VulkanPipelineStateManager
	{
	public:
		void Create(VkDevice device);
		void Destroy(VkDevice device);

		VulkanPipelineState* FindOrCreateGraphicsPipelineState(VkDevice device, const GraphicsPipelineDescription& desc);
		VulkanPipelineState* FindOrCreateComputePipelineState(VkDevice device, const ComputePipelineDescription& desc);

	private:
		VulkanPipelineState* CreateGraphicsPipelineState(VkDevice device, const GraphicsPipelineDescription& desc);
		VulkanPipelineState* CreateComputePipelineState(VkDevice device, const ComputePipelineDescription& desc);

		std::unordered_map<GraphicsPipelineDescription, VulkanPipelineState*> m_GraphicsPipelineCache;
		std::unordered_map<ComputePipelineDescription, VulkanPipelineState*> m_ComputePipelineCache;
		VkPipelineCache m_PipelineCache;
	};
}

#include "graphicshal/vulkan/vulkanpipelinestatemanager.inl"

#endif // KH_GFXAPI_VULKAN