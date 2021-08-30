#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanadapter.hpp"
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/hal/vulkan/vulkanutils.hpp"
#include "system/assert.h"

namespace Khan
{
	VulkanRenderDevice::VulkanRenderDevice(const DisplayAdapter& adapter)
		: RenderDevice(adapter, &m_TransientResourceManager)
		, m_TransientResourceManager(*this)
		, m_BarrierRecorder(adapter)
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
		m_UploadManager.Create(m_MemoryManager.VulkanMemoryAllocator());
		m_SemaphoreAllocator.Create(m_Device);

		// TODO: Create a couple of these (maybe equal to the number of CPU cores)
		m_Contexts.push_back(new VulkanRenderContext(*this));

		m_GraphicsCommandPool.Create(m_Device, m_Adapter.GetQueueFamilyIndices()[QueueType_Graphics]);
		m_CopyCommandPool.Create(m_Device, m_Adapter.GetQueueFamilyIndices()[QueueType_Copy]);
	}

	VulkanRenderDevice::~VulkanRenderDevice()
	{
		m_CopyCommandPool.Destroy();
		m_GraphicsCommandPool.Destroy();

		for (RenderContext* context : m_Contexts)
		{
			delete context;
		}

		m_SemaphoreAllocator.Destroy();
		m_UploadManager.Destroy();
		m_UniformBufferAllocator.Destroy(m_MemoryManager.VulkanMemoryAllocator());
		m_TransientResourceManager.Destroy();
		m_PipelineStateManager.Destroy(m_Device);
		m_PhysicalRenderPassManager.Destroy(m_Device);
		m_MemoryManager.Destroy();
		vkDestroyDevice(m_Device, nullptr);
	}

	Buffer* VulkanRenderDevice::CreateBuffer(const BufferDesc& desc, const void* initData)
	{
		VulkanBuffer* buffer = m_MemoryManager.CreateBuffer(desc);

		if (!initData)
		{
			return buffer;
		}

		VkCommandBuffer cpyCmdBuf = m_CopyCommandPool.AllocateCommandBuffer();

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(cpyCmdBuf, &beginInfo), "[VULKAN] Failed to begin cpy command buffer.");

		uint32_t offset = m_UploadManager.Upload(initData, desc.m_Size);

		m_BarrierRecorder.RecordBarrier(*buffer, ResourceState_CopyDestination, QueueType_Copy);
		m_BarrierRecorder.Flush(cpyCmdBuf);

		VkBufferCopy copy;
		copy.srcOffset = offset;
		copy.dstOffset = 0;
		copy.size = desc.m_Size;

		vkCmdCopyBuffer(cpyCmdBuf, m_UploadManager.CurrentBuffer(), buffer->GetVulkanBuffer(), 1, &copy);

		m_BarrierRecorder.RecordBarrier(*buffer, desc.m_Flags & BufferFlag_AllowVertices ? ResourceState_VertexBuffer : ResourceState_IndexBuffer, QueueType_Graphics);
		m_BarrierRecorder.Flush(cpyCmdBuf);

		VK_ASSERT(vkEndCommandBuffer(cpyCmdBuf), "[VULKAN] Failed to end cpy command buffer.");

		//=========================================================================================================================================================

		VkCommandBuffer gfxCmdBuf = m_GraphicsCommandPool.AllocateCommandBuffer();

		//VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(gfxCmdBuf, &beginInfo), "[VULKAN] Failed to begin gfx command buffer.");

		buffer->SetQueue(QueueType_Copy);
		buffer->SetState(ResourceState_CopyDestination);

		m_BarrierRecorder.RecordBarrier(*buffer, desc.m_Flags & BufferFlag_AllowVertices ? ResourceState_VertexBuffer : ResourceState_IndexBuffer, QueueType_Graphics);
		m_BarrierRecorder.Flush(gfxCmdBuf);

		VK_ASSERT(vkEndCommandBuffer(gfxCmdBuf), "[VULKAN] Failed to end gfx command buffer.");

		//=========================================================================================================================================================

		VkSemaphore semaphore = m_SemaphoreAllocator.AllocateSemaphore();

		VkSubmitInfo cpySubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		cpySubmitInfo.commandBufferCount = 1;
		cpySubmitInfo.pCommandBuffers = &cpyCmdBuf;
		cpySubmitInfo.signalSemaphoreCount = 1;
		cpySubmitInfo.pSignalSemaphores = &semaphore;

		VK_ASSERT(vkQueueSubmit(m_CommandQueues[QueueType_Copy], 1, &cpySubmitInfo, VK_NULL_HANDLE), "[VULKAN] Failed to submit cpy command buffer.");

		VkPipelineStageFlags semaphoreStages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		VkSubmitInfo gfxSubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		gfxSubmitInfo.waitSemaphoreCount = 1;
		gfxSubmitInfo.pWaitSemaphores = &semaphore;
		gfxSubmitInfo.pWaitDstStageMask = &semaphoreStages;
		gfxSubmitInfo.commandBufferCount = 1;
		gfxSubmitInfo.pCommandBuffers = &gfxCmdBuf;

		VK_ASSERT(vkQueueSubmit(m_CommandQueues[QueueType_Graphics], 1, &gfxSubmitInfo, VK_NULL_HANDLE), "[VULKAN] Failed to submit gfx command buffer.");

		return buffer;
	}

	BufferView* VulkanRenderDevice::CreateBufferView(Buffer* buffer, const BufferViewDesc& desc)
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

	Texture* VulkanRenderDevice::CreateTexture(const TextureDesc& desc, const void* initData)
	{
		VulkanTexture* texture = m_MemoryManager.CreateTexture(desc);

		if (!initData)
		{
			return texture;
		}

		VkCommandBuffer cpyCmdBuf = m_CopyCommandPool.AllocateCommandBuffer();

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(cpyCmdBuf, &beginInfo), "[VULKAN] Failed to begin cpy command buffer.");

		uint32_t offset = m_UploadManager.Upload(initData, desc.m_Width * desc.m_Height * 4);

		m_BarrierRecorder.RecordBarrier(*texture, ResourceState_CopyDestination, QueueType_Copy);
		m_BarrierRecorder.Flush(cpyCmdBuf);

		VkBufferImageCopy copy;
		copy.bufferOffset = offset;
		copy.bufferRowLength = 0;
		copy.bufferImageHeight = 0;
		copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy.imageSubresource.mipLevel = 0;
		copy.imageSubresource.baseArrayLayer = 0;
		copy.imageSubresource.layerCount = 1;
		copy.imageOffset = { 0, 0, 0 };
		copy.imageExtent =
		{
			desc.m_Width,
			desc.m_Height,
			1
		};

		vkCmdCopyBufferToImage(cpyCmdBuf, m_UploadManager.CurrentBuffer(), texture->VulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

		m_BarrierRecorder.RecordBarrier(*texture, ResourceState_PixelShaderAccess, QueueType_Graphics);
		m_BarrierRecorder.Flush(cpyCmdBuf);

		VK_ASSERT(vkEndCommandBuffer(cpyCmdBuf), "[VULKAN] Failed to end cpy command buffer.");

		//=========================================================================================================================================================

		VkCommandBuffer gfxCmdBuf = m_GraphicsCommandPool.AllocateCommandBuffer();

		//VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(gfxCmdBuf, &beginInfo), "[VULKAN] Failed to begin gfx command buffer.");

		texture->SetQueue(QueueType_Copy);
		texture->SetState(ResourceState_CopyDestination);

		m_BarrierRecorder.RecordBarrier(*texture, ResourceState_PixelShaderAccess, QueueType_Graphics);
		m_BarrierRecorder.Flush(gfxCmdBuf);

		VK_ASSERT(vkEndCommandBuffer(gfxCmdBuf), "[VULKAN] Failed to end gfx command buffer.");

		//=========================================================================================================================================================

		VkSemaphore semaphore = m_SemaphoreAllocator.AllocateSemaphore();

		VkSubmitInfo cpySubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		cpySubmitInfo.commandBufferCount = 1;
		cpySubmitInfo.pCommandBuffers = &cpyCmdBuf;
		cpySubmitInfo.signalSemaphoreCount = 1;
		cpySubmitInfo.pSignalSemaphores = &semaphore;

		VK_ASSERT(vkQueueSubmit(m_CommandQueues[QueueType_Copy], 1, &cpySubmitInfo, VK_NULL_HANDLE), "[VULKAN] Failed to submit cpy command buffer.");

		VkPipelineStageFlags semaphoreStages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		VkSubmitInfo gfxSubmitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		gfxSubmitInfo.waitSemaphoreCount = 1;
		gfxSubmitInfo.pWaitSemaphores = &semaphore;
		gfxSubmitInfo.pWaitDstStageMask = &semaphoreStages;
		gfxSubmitInfo.commandBufferCount = 1;
		gfxSubmitInfo.pCommandBuffers = &gfxCmdBuf;

		VK_ASSERT(vkQueueSubmit(m_CommandQueues[QueueType_Graphics], 1, &gfxSubmitInfo, VK_NULL_HANDLE), "[VULKAN] Failed to submit gfx command buffer.");

		return texture;
	}

	TextureView* VulkanRenderDevice::CreateTextureView(Texture* texture, const TextureViewDesc& desc)
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

	Shader* VulkanRenderDevice::CreateShader(const ShaderDesc& desc)
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

	PhysicalRenderPass* VulkanRenderDevice::CreatePhysicalRenderPass(const PhysicalRenderPassDescription& desc)
	{
		return m_PhysicalRenderPassManager.FindOrCreatePhysicalRenderPass(m_Device, desc);
	}

	RenderPipelineState* VulkanRenderDevice::CreateGraphicsPipelineState(const GraphicsPipelineDescription& desc)
	{
		return m_PipelineStateManager.FindOrCreateGraphicsPipelineState(m_Device, desc);
	}

	RenderPipelineState* VulkanRenderDevice::CreateComputePipelineState(const ComputePipelineDescription& desc)
	{
		return m_PipelineStateManager.FindOrCreateComputePipelineState(m_Device, desc);
	}

	void VulkanRenderDevice::DestroyBuffer(Buffer* buffer)
	{
		m_MemoryManager.DestroyBuffer(reinterpret_cast<VulkanBuffer*>(buffer));
	}

	void VulkanRenderDevice::DestroyBufferView(BufferView* view)
	{
		VulkanBufferView* vulkanView = reinterpret_cast<VulkanBufferView*>(view);
		vkDestroyBufferView(m_Device, vulkanView->GetVulkanBufferView(), nullptr);
		delete vulkanView;
	}

	void VulkanRenderDevice::DestroyTexture(Texture* texture)
	{
		m_MemoryManager.DestroyTexture(reinterpret_cast<VulkanTexture*>(texture));
	}

	void VulkanRenderDevice::DestroyTextureView(TextureView* view)
	{
		VulkanTextureView* vulkanView = reinterpret_cast<VulkanTextureView*>(view);
		vkDestroyImageView(m_Device, vulkanView->VulkanImageView(), nullptr);
		delete vulkanView;
	}

	void VulkanRenderDevice::DestroyShader(Shader* shader)
	{
		VulkanShader* vulkanShader = reinterpret_cast<VulkanShader*>(shader);
		spvReflectDestroyShaderModule(&vulkanShader->GetReflection());
		vkDestroyShaderModule(m_Device, vulkanShader->GetShaderInfo().module, nullptr);
		delete vulkanShader;
	}

	void VulkanRenderDevice::WaitIdle()
	{
		VK_ASSERT(vkDeviceWaitIdle(m_Device), "[VULKAN] Failed to wait device idle.");
	}

	void VulkanRenderDevice::OnFlip(uint32_t frameIndex)
	{
		++m_FrameCounter;

		m_SemaphoreAllocator.ResetFrame(frameIndex);
		m_UploadManager.ResetFrame(frameIndex);
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

	void VulkanRenderDevice::SubmitCommands(VkCommandBuffer commandBuffer, const RenderGraph::Node* rgSubmitInfo)
	{
		std::lock_guard lock(m_CommandSubmitLock);

		m_CommandSubmissions.insert({ rgSubmitInfo->GlobalExecutionIndex(), { commandBuffer, rgSubmitInfo } });
		if (rgSubmitInfo->IsSyncSignalRequired())
		{
			m_BufferIdToSemaphoreMap.emplace(rgSubmitInfo->GlobalExecutionIndex(), m_SemaphoreAllocator.AllocateSemaphore());
		}
	}

	void VulkanRenderDevice::FlushCommands()
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
				std::vector<VkSemaphore> waitSemaphores;
				std::vector<VkPipelineStageFlags> waitStages;

				VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

				for (const RenderGraph::Node* syncNode : info->NodesToSyncWith())
				{
					waitSemaphores.push_back(m_BufferIdToSemaphoreMap[syncNode->GlobalExecutionIndex()]);
					waitStages.push_back(s_WaitStagesForQueues[syncNode->m_ExecutionQueueIndex]);
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