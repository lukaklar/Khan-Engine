#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanadapter.hpp"
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkanutils.hpp"
#include "system/assert.h"

namespace Khan
{
	RenderDevice::RenderDevice(const DisplayAdapter& adapter)
		: m_Adapter(adapter)
		, m_TransientResourceManager(*this)
		, m_RenderGraph(&m_TransientResourceManager)
	{
		VkDeviceQueueCreateInfo queueInfos[QueueType_Count];
		float queuePriority = 1.0f;

		for (uint32_t i = 0; i < QueueType_Count; ++i)
		{
			queueInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfos[i].pNext = nullptr;
			queueInfos[i].flags = 0;
			queueInfos[i].queueFamilyIndex = adapter.GetQueueFamilyIndices()[i];
			queueInfos[i].queueCount = 1;
			queueInfos[i].pQueuePriorities = &queuePriority;
		}

		VkDeviceCreateInfo deviceInfo;
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = nullptr;
		deviceInfo.flags = 0;
		deviceInfo.queueCreateInfoCount = QueueType_Count;
		deviceInfo.pQueueCreateInfos = queueInfos;
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = nullptr;
		deviceInfo.enabledExtensionCount = static_cast<uint32_t>(adapter.GetSupportedExtensions().size());
		deviceInfo.ppEnabledExtensionNames = adapter.GetSupportedExtensions().data();
		deviceInfo.pEnabledFeatures = &adapter.GetEnabledFeatures();

		VK_ASSERT(vkCreateDevice(m_Adapter.VulkanAdapter(), &deviceInfo, nullptr, &m_Device), "[VULKAN] Failed to create device.");

		for (uint32_t i = 0; i < QueueType_Count; ++i)
		{
			vkGetDeviceQueue(m_Device, adapter.GetQueueFamilyIndices()[i], 0, &m_CommandQueues[i]);
		}

		m_MemoryManager.Create(m_Adapter.VulkanAdapter(), m_Device);
		m_PipelineStateManager.Create(m_Device);
		m_TransientResourceManager.Create(m_MemoryManager.VulkanMemoryAllocator());
		m_UniformBufferAllocator.Create(m_MemoryManager.VulkanMemoryAllocator());
		m_SemaphoreAllocator.Create(m_Device);

		// TODO: Create a couple of these (maybe equal to the number of CPU cores)
		m_Contexts.push_back(new RenderContext(*this));
	}

	RenderDevice::~RenderDevice()
	{
		for (RenderContext* context : m_Contexts)
		{
			delete context;
		}

		m_SemaphoreAllocator.Destroy();
		m_UniformBufferAllocator.Destroy(m_MemoryManager.VulkanMemoryAllocator());
		m_PipelineStateManager.Destroy(m_Device);
		m_PhysicalRenderPassManager.Destroy(m_Device);
		m_MemoryManager.Destroy();
		vkDestroyDevice(m_Device, nullptr);
	}

	Buffer* RenderDevice::CreateBuffer(const BufferDesc& desc)
	{
		return m_MemoryManager.CreateBuffer(desc);
	}

	BufferView* RenderDevice::CreateBufferView(Buffer* buffer, const BufferViewDesc& desc)
	{
		VkBufferViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO };
		viewInfo.buffer = reinterpret_cast<VulkanBuffer*>(buffer)->GetVulkanBuffer();
		viewInfo.format = PixelFormatToVulkanFormat(desc.m_Format);
		viewInfo.offset = desc.m_Offset;
		viewInfo.range = desc.m_Range;

		VkBufferView bufferView;
		VK_ASSERT(vkCreateBufferView(m_Device, &viewInfo, nullptr, &bufferView), "[VULKAN] Failed to create buffer view.");

		return new VulkanBufferView(bufferView, *buffer, desc);
	}

	Texture* RenderDevice::CreateTexture(const TextureDesc& desc)
	{
		return m_MemoryManager.CreateTexture(desc);
	}

	TextureView* RenderDevice::CreateTextureView(Texture* texture, const TextureViewDesc& desc)
	{
		VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = reinterpret_cast<VulkanTexture*>(texture)->VulkanImage();
		viewInfo.viewType = TextureViewTypeToVulkanImageViewType(desc.m_Type);
		viewInfo.format = PixelFormatToVulkanFormat(desc.m_Format);
		viewInfo.subresourceRange.aspectMask = PixelFormatToVulkanAspectMask(desc.m_Format);
		viewInfo.subresourceRange.baseMipLevel = desc.m_BaseMipLevel;
		viewInfo.subresourceRange.levelCount = desc.m_LevelCount;
		viewInfo.subresourceRange.baseArrayLayer = desc.m_BaseArrayLayer;
		viewInfo.subresourceRange.layerCount = desc.m_LayerCount;

		VkImageView imageView;
		VK_ASSERT(vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView), "[VULKAN] Failed to create image view.");

		return new VulkanTextureView(imageView, *texture, desc);
	}

	Shader* RenderDevice::CreateShader(const ShaderDesc& desc)
	{
		VkShaderModuleCreateInfo shaderModuleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		shaderModuleInfo.codeSize = desc.m_BytecodeSize;
		shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(desc.m_Bytecode);

		VkShaderModule shaderModule;
		VK_ASSERT(vkCreateShaderModule(m_Device, &shaderModuleInfo, nullptr, &shaderModule), "[VULKAN] Failed to create shader module.");

		VkPipelineShaderStageCreateInfo shaderStageInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		shaderStageInfo.stage = ShaderTypeToVulkanShaderStage(desc.m_Type);
		shaderStageInfo.module = shaderModule;
		shaderStageInfo.pName = desc.m_EntryPoint;

		SpvReflectShaderModule reflection;
		SpvReflectResult result = spvReflectCreateShaderModule(desc.m_BytecodeSize, desc.m_Bytecode, &reflection);

		return new VulkanShader(shaderStageInfo, reflection, desc.m_EntryPoint, desc.m_Type);
	}

	PhysicalRenderPass* RenderDevice::CreatePhysicalRenderPass(const PhysicalRenderPassDescription& desc)
	{
		return m_PhysicalRenderPassManager.FindOrCreatePhysicalRenderPass(m_Device, desc);
	}

	RenderPipelineState* RenderDevice::CreateGraphicsPipelineState(const GraphicsPipelineDescription& desc)
	{
		return m_PipelineStateManager.FindOrCreateGraphicsPipelineState(m_Device, desc);
	}

	RenderPipelineState* RenderDevice::CreateComputePipelineState(const ComputePipelineDescription& desc)
	{
		return m_PipelineStateManager.FindOrCreateComputePipelineState(m_Device, desc);
	}

	void RenderDevice::DestroyBuffer(Buffer* buffer)
	{
		m_MemoryManager.DestroyBuffer(reinterpret_cast<VulkanBuffer*>(buffer));
	}

	void RenderDevice::DestroyBufferView(BufferView* view)
	{
		VulkanBufferView* vulkanView = reinterpret_cast<VulkanBufferView*>(view);
		vkDestroyBufferView(m_Device, vulkanView->GetVulkanBufferView(), nullptr);
		delete vulkanView;
	}

	void RenderDevice::DestroyTexture(Texture* texture)
	{
		m_MemoryManager.DestroyTexture(reinterpret_cast<VulkanTexture*>(texture));
	}

	void RenderDevice::DestroyTextureView(TextureView* view)
	{
		VulkanTextureView* vulkanView = reinterpret_cast<VulkanTextureView*>(view);
		vkDestroyImageView(m_Device, vulkanView->VulkanImageView(), nullptr);
		delete vulkanView;
	}

	void RenderDevice::DestroyShader(Shader* shader)
	{
		VulkanShader* vulkanShader = reinterpret_cast<VulkanShader*>(shader);
		spvReflectDestroyShaderModule(&vulkanShader->GetReflection());
		vkDestroyShaderModule(m_Device, vulkanShader->GetShaderInfo().module, nullptr);
		delete vulkanShader;
	}

	void RenderDevice::OnFlip(uint32_t frameIndex)
	{
		++m_FrameCounter;

		m_SemaphoreAllocator.ResetFrame(frameIndex);
		m_UniformBufferAllocator.ResetFrame(frameIndex);
		m_TransientResourceManager.ResetFrame(frameIndex);

		for (RenderContext* context : m_Contexts)
		{
			context->ResetFrame(frameIndex);
		}

		m_RenderGraph.Reset();
		m_CommandSubmissions.clear();
		m_BufferIdToSemaphoreMap.clear();
	}

	void RenderDevice::SubmitCommands(VkCommandBuffer commandBuffer, const RenderGraph::Node* rgSubmitInfo)
	{
		std::lock_guard lock(m_CommandSubmitLock);

		m_CommandSubmissions.insert({ rgSubmitInfo->GlobalExecutionIndex(), { commandBuffer, rgSubmitInfo } });
		if (rgSubmitInfo->IsSyncSignalRequired())
		{
			m_BufferIdToSemaphoreMap.emplace(rgSubmitInfo->GlobalExecutionIndex(), m_SemaphoreAllocator.AllocateSemaphore());
		}
	}

	void RenderDevice::FlushCommands()
	{
		// TODO: Tweak this function to submit more submit infos instead of just one each time synchronization is needed
		static const VkPipelineStageFlags s_WaitStagesForQueues[QueueType_Count] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT };

		std::vector<VkCommandBuffer> commandBuffers[QueueType_Count];

		for (auto [cmdBufId, data] : m_CommandSubmissions)
		{
			auto [cmdBuf, info] = data;

			commandBuffers[info->m_ExecutionQueueIndex].push_back(cmdBuf);

			if (info->IsSyncSignalRequired() || !info->NodesToSyncWith().empty())
			{
				std::vector<VkSemaphore> waitSemaphores(info->NodesToSyncWith().size());
				std::vector<VkPipelineStageFlags> waitStages(info->NodesToSyncWith().size());

				VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

				for (const RenderGraph::Node* syncNode : info->NodesToSyncWith())
				{
					waitSemaphores.push_back(m_BufferIdToSemaphoreMap[syncNode->GlobalExecutionIndex()]);
					waitStages.push_back(s_WaitStagesForQueues[syncNode->GlobalExecutionIndex()]);
				}

				submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
				submitInfo.pWaitSemaphores = waitSemaphores.data();
				submitInfo.pWaitDstStageMask = waitStages.data();
				submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers[info->m_ExecutionQueueIndex].size());
				submitInfo.pCommandBuffers = commandBuffers[info->m_ExecutionQueueIndex].data();

				if (info->IsSyncSignalRequired())
				{
					submitInfo.signalSemaphoreCount = 1;
					submitInfo.pSignalSemaphores = &m_BufferIdToSemaphoreMap[cmdBufId];
				}

				VK_ASSERT(vkQueueSubmit(m_CommandQueues[info->m_ExecutionQueueIndex], 1, &submitInfo, VK_NULL_HANDLE), "[VULKAN] Failed to submit to queue.");

				commandBuffers[info->m_ExecutionQueueIndex].clear();
			}
		}

		for (uint32_t i = 0; i < QueueType_Count; ++i)
		{
			auto& cmdBuffers = commandBuffers[i];
			if (!cmdBuffers.empty())
			{
				VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
				submitInfo.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size());
				submitInfo.pCommandBuffers = cmdBuffers.data();

				VK_ASSERT(vkQueueSubmit(m_CommandQueues[i], 1, &submitInfo, VK_NULL_HANDLE), "[VULKAN] Failed to submit to queue.");
			}
		}
	}
}

#endif // KH_GFXAPI_VULKAN