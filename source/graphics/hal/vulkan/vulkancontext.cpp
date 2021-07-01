#include "graphics/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/commandtype.hpp"
#include "graphics/hal/constantbuffer.hpp"
#include "graphics/hal/vulkan/vulkanadapter.hpp"
#include "graphics/hal/vulkan/vulkancontext.hpp"
#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/renderpass.hpp"
#include "system/assert.h"

namespace Khan
{
	RenderContext::RenderContext(RenderDevice& device)
		: m_Device(device)
		, m_BarrierRecorder(device.GetAdapter())
		, m_DescriptorPool(device.VulkanDevice())
		, m_DescriptorUpdater(device.VulkanDevice())
		, m_CurrentFramebuffers(&m_Framebuffers[0])
	{
		m_CommandPools[0].Create(device.VulkanDevice(), device.GetAdapter().GetQueueFamilyIndices()[QueueType_Graphics]);
		m_CommandPools[1].Create(device.VulkanDevice(), device.GetAdapter().GetQueueFamilyIndices()[QueueType_Compute]);
	}

	RenderContext::~RenderContext()
	{
		m_CommandPools[0].Destroy();
		m_CommandPools[1].Destroy();
	}

	void RenderContext::BeginRecording(const RenderPass& pass)
	{
		m_ExecutingPass = &pass;

		for (uint32_t i = 0; i < K_MAX_VERTEX_DATA_STREAMS; ++i)
		{
			m_VertexBuffers[i] = nullptr;
			m_VBOffsets[i] = 0;
			m_VBDirty[i] = false;
		}

		m_IndexBuffer = nullptr;
		m_IndexBufferDirty = false;
		m_ShouldBindIndexBuffer = false;

		m_PipelineState = nullptr;
		m_PipelineDirty = false;

		m_ViewportDirty = false;
		m_ScissorDirty = false;

		for (uint32_t set = 0; set < ResourceBindFrequency_Count; ++set)
		{
			for (uint32_t i = 0; i < K_MAX_CBV; ++i)
			{
				m_CBuffers[set][i] = nullptr;
			}
			for (uint32_t i = 0; i < K_MAX_SRV; ++i)
			{
				m_SRVBuffers[set][i] = nullptr;
				m_SRVTextures[set][i] = nullptr;
			}
			for (uint32_t i = 0; i < K_MAX_UAV; ++i)
			{
				m_UAVBuffers[set][i] = nullptr;
				m_UAVTextures[set][i] = nullptr;
			}
		}

		m_FirstDirtySet = ResourceBindFrequency_Count;

#ifdef KH_DEBUG
		m_IsRenderPassInProgress = false;
#endif // KH_DEBUG

		m_CommandBuffer = m_CommandPools[pass.GetExecutionQueue()].AllocateCommandBuffer();

		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_ASSERT(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo), "[VULKAN] Failed to begin command buffer.");
	}

	void RenderContext::EndRecording()
	{
		RenderGraph::Node* rgNode = m_Device.GetRenderGraph().GetPassNode(m_ExecutingPass);

		for (BufferView* view : rgNode->AllBuffers())
		{
			std::pair<ResourceState, uint64_t> barrierData = rgNode->GetBarrierDataForResource(view->GetBuffer());
			m_BarrierRecorder.RecordBarrier(*reinterpret_cast<VulkanBufferView*>(view), barrierData.first, (QueueType)barrierData.second);
		}

		for (TextureView* view : rgNode->AllTextures())
		{
			std::pair<ResourceState, uint64_t> barrierData = rgNode->GetBarrierDataForResource(view->GetTexture());
			m_BarrierRecorder.RecordBarrier(*reinterpret_cast<VulkanTextureView*>(view), barrierData.first, (QueueType)barrierData.second);
		}

		m_BarrierRecorder.Flush(m_CommandBuffer);

		vkEndCommandBuffer(m_CommandBuffer);

		m_Device.SubmitCommands(m_CommandBuffer, rgNode);
	}

	void RenderContext::BeginPhysicalRenderPass(const PhysicalRenderPass& renderPass, TextureView* renderTargets[], TextureView* depthStencilBuffer)
	{
		KH_ASSERT(!m_IsRenderPassInProgress, "Render pass already in progress. Cannot begin another render pass until the first one ends.");
#ifdef KH_DEBUG
		m_IsRenderPassInProgress = true;
#endif // KH_DEBUG

		VkClearValue clearValues[K_MAX_RENDER_TARGETS + 1];
		uint32_t clearValueCount = 0;
		VkImageView attachments[K_MAX_RENDER_TARGETS + 1];
		uint32_t attachmentCount = renderPass.GetDesc().m_RenderTargetCount;
		uint32_t width = UINT32_MAX, height = UINT32_MAX;
		
		for (uint32_t i = 0; i < renderPass.GetDesc().m_RenderTargetCount; ++i)
		{
			VulkanTextureView* textureView = reinterpret_cast<VulkanTextureView*>(renderTargets[i]);

			attachments[i] = textureView->VulkanImageView();
			width = std::min(width, textureView->GetTexture().GetDesc().m_Width);
			height = std::min(height, textureView->GetTexture().GetDesc().m_Height);

			StartAccessType attachmentStartAccess = renderPass.GetDesc().m_RenderTargets[i].m_StartAccess;
			if (attachmentStartAccess == StartAccessType::Clear)
			{
				const glm::vec4& color = renderPass.GetRenderTargetClearColor(i);
				clearValues[clearValueCount++] = { color.r, color.g, color.b, color.a };
			}
			else if (attachmentStartAccess == StartAccessType::Keep)
			{
				m_BarrierRecorder.RecordBarrier(*textureView, ResourceState_RenderTarget, QueueType_Graphics);
			}

			Texture& texture = textureView->GetTexture();
			texture.SetState(ResourceState_RenderTarget);
			texture.SetQueue(QueueType_Graphics);
		}
		
		if (depthStencilBuffer)
		{
			VulkanTextureView* textureView = reinterpret_cast<VulkanTextureView*>(depthStencilBuffer);

			attachments[attachmentCount++] = textureView->VulkanImageView();
			width = std::min(width, textureView->GetTexture().GetDesc().m_Width);
			height = std::min(height, textureView->GetTexture().GetDesc().m_Height);

			StartAccessType depthStartAccess = renderPass.GetDesc().m_DepthStencil.m_DepthStartAccess;
			StartAccessType stencilStartAccess = renderPass.GetDesc().m_DepthStencil.m_StencilStartAccess;
			if (depthStartAccess == StartAccessType::Clear || stencilStartAccess == StartAccessType::Clear)
			{
				VkClearDepthStencilValue& clearValue = clearValues[clearValueCount++].depthStencil;
				clearValue = { renderPass.GetDepthClearValue(), renderPass.GetStencilClearValue() };
			}
			else if (depthStartAccess == StartAccessType::Keep || stencilStartAccess == StartAccessType::Keep)
			{
				// TODO: Mora da se vidi ovde
				m_BarrierRecorder.RecordBarrier(*textureView, ResourceState_DepthWriteStencilWrite, QueueType_Graphics);
			}

			Texture& texture = textureView->GetTexture();
			texture.SetState(ResourceState_DepthWriteStencilWrite);
			texture.SetQueue(QueueType_Graphics);
		}

		m_BarrierRecorder.Flush(m_CommandBuffer);
		
		VkFramebufferCreateInfo framebufferInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = reinterpret_cast<const VulkanPhysicalRenderPass&>(renderPass).VulkanRenderPass();
		framebufferInfo.attachmentCount = attachmentCount;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;
		
		VkFramebuffer framebuffer;
		VK_ASSERT(vkCreateFramebuffer(m_Device.VulkanDevice(), &framebufferInfo, nullptr, &framebuffer), "[VULKAN] Failed to create framebuffer.");
		m_CurrentFramebuffers->push_back(framebuffer);
		
		VkRenderPassBeginInfo renderPassBegin = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassBegin.renderPass = reinterpret_cast<const VulkanPhysicalRenderPass&>(renderPass).VulkanRenderPass();
		renderPassBegin.framebuffer = framebuffer;
		renderPassBegin.renderArea = { { 0, 0 }, { width, height } };
		renderPassBegin.clearValueCount = clearValueCount;
		renderPassBegin.pClearValues = clearValues;
		
		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
	}

	void RenderContext::EndPhysicalRenderPass()
	{
		KH_ASSERT(m_IsRenderPassInProgress, "No render pass is in progress. You cannot end a render pass that hasn't begun yet.");
#ifdef KH_DEBUG
		m_IsRenderPassInProgress = false;
#endif // KH_DEBUG

		vkCmdEndRenderPass(m_CommandBuffer);
	}

	void RenderContext::SetVertexBuffer(uint32_t location, Buffer* vertexBuffer, uint32_t offset)
	{
		VulkanBuffer* vb = reinterpret_cast<VulkanBuffer*>(vertexBuffer);
		if (m_VertexBuffers[location] != vb)
		{
			m_VertexBuffers[location] = vb;
			m_VBOffsets[location] = offset;
			m_VBDirty[location] = true;
		}
	}

	void RenderContext::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset, bool useTwoByteIndex)
	{
		VulkanBuffer* ib = reinterpret_cast<VulkanBuffer*>(indexBuffer);
		if (m_IndexBuffer != ib)
		{
			m_IndexBuffer = ib;
			m_IBOffset = offset;
			m_IndexBufferDirty = true;
			m_IndexType = useTwoByteIndex ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
		}
	}

	void RenderContext::SetConstantBuffer(ResourceBindFrequency frequency, uint32_t binding, ConstantBuffer* cbuffer)
	{
		if (m_CBuffers[frequency][binding] != cbuffer)
		{
			m_CBuffers[frequency][binding] = cbuffer;
			m_FirstDirtySet = frequency < m_FirstDirtySet ? frequency : m_FirstDirtySet;
		}
	}

	void RenderContext::SetSRVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture)
	{
		if (m_SRVTextures[frequency][binding] != texture)
		{
			m_SRVTextures[frequency][binding] = reinterpret_cast<VulkanTextureView*>(texture);
			m_SRVBuffers[frequency][binding] = nullptr;
			m_FirstDirtySet = frequency < m_FirstDirtySet ? frequency : m_FirstDirtySet;
		}
	}

	void RenderContext::SetSRVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer)
	{
		if (m_SRVBuffers[frequency][binding] != buffer)
		{
			m_SRVBuffers[frequency][binding] = reinterpret_cast<VulkanBufferView*>(buffer);
			m_SRVTextures[frequency][binding] = nullptr;
			m_FirstDirtySet = frequency < m_FirstDirtySet ? frequency : m_FirstDirtySet;
		}
	}

	void RenderContext::SetUAVTexture(ResourceBindFrequency frequency, uint32_t binding, TextureView* texture)
	{
		if (m_UAVTextures[frequency][binding] != texture)
		{
			m_UAVTextures[frequency][binding] = reinterpret_cast<VulkanTextureView*>(texture);
			m_UAVBuffers[frequency][binding] = nullptr;
			m_FirstDirtySet = frequency < m_FirstDirtySet ? frequency : m_FirstDirtySet;
		}
	}

	void RenderContext::SetUAVBuffer(ResourceBindFrequency frequency, uint32_t binding, BufferView* buffer)
	{
		if (m_UAVBuffers[frequency][binding] != buffer)
		{
			m_UAVBuffers[frequency][binding] = reinterpret_cast<VulkanBufferView*>(buffer);
			m_UAVTextures[frequency][binding] = nullptr;
			m_FirstDirtySet = frequency < m_FirstDirtySet ? frequency : m_FirstDirtySet;
		}
	}

	void RenderContext::SetPipelineState(const RenderPipelineState& pipelineState)
	{
		if (m_PipelineState != &pipelineState)
		{
			m_PipelineState = reinterpret_cast<const VulkanPipelineState*>(&pipelineState);
			m_PipelineDirty = true;
		}
	}

	void RenderContext::SetViewport(float left, float top, float width, float height, float minDepth, float maxDepth)
	{
		m_Viewport.x = left;
		m_Viewport.y = top;
		m_Viewport.width = width;
		m_Viewport.height = height;
		m_Viewport.minDepth = minDepth;
		m_Viewport.maxDepth = maxDepth;
		m_ViewportDirty = true;
	}

	void RenderContext::SetScissor(int32_t left, int32_t top, uint32_t width, uint32_t height)
	{
		m_Scissor.offset.x = left;
		m_Scissor.offset.y = top;
		m_Scissor.extent.width = width;
		m_Scissor.extent.height = height;
		m_ScissorDirty = true;
	}

	void RenderContext::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		KH_ASSERT(m_IsRenderPassInProgress, "Cannot issue draw commands until a valid render pass is in progress. Please begin a render pass before drawing.");
		m_CommandType = CommandType::Draw;
		BindPipeline();
		InsertBarriers();
		BindDynamicStates();
		m_ShouldBindIndexBuffer = false;
		BindResources();
		vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void RenderContext::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		KH_ASSERT(m_IsRenderPassInProgress, "Cannot issue draw commands until a valid render pass is in progress. Please begin a render pass before drawing.");
		m_CommandType = CommandType::Draw;
		BindPipeline();
		InsertBarriers();
		BindDynamicStates();
		m_ShouldBindIndexBuffer = true;
		BindResources();
		vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void RenderContext::Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ)
	{
		m_CommandType = CommandType::Dispatch;
		BindPipeline();
		InsertBarriers();
		BindResources();
		vkCmdDispatch(m_CommandBuffer, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
	}

	void RenderContext::ResetFrame(uint32_t frameIndex)
	{
		m_CurrentFramebuffers = &m_Framebuffers[frameIndex];
		for (VkFramebuffer framebuffer : *m_CurrentFramebuffers)
		{
			vkDestroyFramebuffer(m_Device.VulkanDevice(), framebuffer, nullptr);
		}
		m_CurrentFramebuffers->clear();

		m_DescriptorPool.ResetFrame(frameIndex);

		for (VulkanCommandPool& commandPool : m_CommandPools)
		{
			commandPool.ResetFrame(frameIndex);
		}
	}

	inline void RenderContext::InsertBarriers()
	{
		if (m_CommandType == CommandType::Draw)
		{
			for (uint32_t i = 0; i < K_MAX_VERTEX_DATA_STREAMS; ++i)
			{
				if (m_VertexBuffers[i] != nullptr && m_VBDirty[i])
				{
					m_BarrierRecorder.RecordBarrier(*m_VertexBuffers[i], ResourceState_VertexBuffer, m_ExecutingPass->GetExecutionQueue());
				}
			}

			if (m_ShouldBindIndexBuffer && m_IndexBufferDirty)
			{
				m_BarrierRecorder.RecordBarrier(*m_IndexBuffer, ResourceState_IndexBuffer, m_ExecutingPass->GetExecutionQueue());
			}
		}

		if (m_FirstDirtySet == ResourceBindFrequency_Count) return;

		ResourceState srvResourceState = m_CommandType == CommandType::Dispatch ? ResourceState_NonPixelShaderAccess : ResourceState_PixelShaderAccess;
		for (uint32_t set = m_FirstDirtySet; set < ResourceBindFrequency_Count; ++set)
		{
			for (uint32_t binding = 0; binding < K_MAX_SRV; ++binding)
			{
				VulkanTextureView* texture = m_SRVTextures[set][binding];
				if (texture != nullptr)
				{
					m_BarrierRecorder.RecordBarrier(*texture, srvResourceState, m_ExecutingPass->GetExecutionQueue());
					continue;
				}

				VulkanBufferView* buffer = m_SRVBuffers[set][binding];
				if (buffer != nullptr)
				{
					m_BarrierRecorder.RecordBarrier(*buffer, srvResourceState, m_ExecutingPass->GetExecutionQueue());
				}
			}

			if (m_CommandType == CommandType::Dispatch)
			{
				for (uint32_t binding = 0; binding < K_MAX_UAV; ++binding)
				{
					VulkanTextureView* texture = m_UAVTextures[set][binding];
					if (texture != nullptr)
					{
						m_BarrierRecorder.RecordBarrier(*texture, ResourceState_UnorderedAccess, m_ExecutingPass->GetExecutionQueue());
						continue;
					}

					VulkanBufferView* buffer = m_UAVBuffers[set][binding];
					if (buffer != nullptr)
					{
						m_BarrierRecorder.RecordBarrier(*buffer, ResourceState_UnorderedAccess, m_ExecutingPass->GetExecutionQueue());
					}
				}
			}
		}

	}

	inline void RenderContext::BindResources()
	{
		if (m_CommandType == CommandType::Draw)
		{
			VkBuffer vbs[K_MAX_VERTEX_DATA_STREAMS];
			uint32_t count = 0;
			uint32_t first = 0;
			for (uint32_t i = 0; i < K_MAX_VERTEX_DATA_STREAMS; ++i)
			{
				if (m_VertexBuffers[i] != nullptr && m_VBDirty[i])
				{
					if (count == 0)
					{
						first = i;
					}
					vbs[count++] = m_VertexBuffers[i]->GetVulkanBuffer();
					m_VBDirty[i] = false;
				}
				else if (count > 0)
				{
					vkCmdBindVertexBuffers(m_CommandBuffer, first, count, vbs + first, m_VBOffsets + first);
					count = 0;
				}
			}

			if (m_ShouldBindIndexBuffer && m_IndexBufferDirty)
			{
				vkCmdBindIndexBuffer(m_CommandBuffer, m_IndexBuffer->GetVulkanBuffer(), m_IBOffset, m_IndexType);
				m_ShouldBindIndexBuffer = false;
				m_IndexBufferDirty = false;
			}
		}

		if (m_FirstDirtySet == ResourceBindFrequency_Count) return;

		m_DescriptorPool.AllocateDescriptorSets(ResourceBindFrequency_Count - m_FirstDirtySet, m_PipelineState->m_DescriptorSetLayout + m_FirstDirtySet, m_DescriptorSets + m_FirstDirtySet);
		m_DescriptorUpdater.Reset();

		for (uint32_t set = m_FirstDirtySet; set < ResourceBindFrequency_Count; ++set)
		{
			m_DescriptorUpdater.SetDescriptorSet(m_DescriptorSets[set]);

			VkBuffer uniformBufferForFrame = m_Device.m_UniformBufferAllocator.CurrentBuffer();
			for (uint32_t binding = 0; binding < K_MAX_CBV; ++binding)
			{
				ConstantBuffer* cbuffer = m_CBuffers[set][binding];
				if (cbuffer != nullptr)
				{
					uint32_t size = cbuffer->Size();
					uint32_t offset = cbuffer->GetDynamicOffset();
					if (cbuffer->IsDirty())
					{
						offset = m_Device.m_UniformBufferAllocator.Upload(cbuffer->Data(), size);
						cbuffer->SetDynamicOffset(offset);
					}
					
					m_DescriptorUpdater.SetUniformBuffer(binding, uniformBufferForFrame, offset, size);
				}
			}

			for (uint32_t binding = 0; binding < K_MAX_SRV; ++binding)
			{
				const VulkanTextureView* texture = m_SRVTextures[set][binding];
				if (texture != nullptr)
				{
					m_DescriptorUpdater.SetSampledImage(binding, texture->VulkanImageView());
					continue;
				}

				const VulkanBufferView* buffer = m_SRVBuffers[set][binding];
				if (buffer != nullptr)
				{
					if (buffer->GetDesc().m_Format == PF_NONE)
					{
						VkBuffer vulkanBuffer = static_cast<const VulkanBuffer&>(buffer->GetBuffer()).GetVulkanBuffer();
						VkDeviceSize offset = buffer->GetDesc().m_Offset;
						VkDeviceSize range = buffer->GetDesc().m_Range;
						m_DescriptorUpdater.SetStorageBuffer(binding, vulkanBuffer, offset, range);
					}
					else
					{
						m_DescriptorUpdater.SetStorageTexelBuffer(binding, buffer->GetVulkanBufferView());
					}
				}
			}

			if (m_CommandType == CommandType::Dispatch)
			{
				for (uint32_t binding = 0; binding < K_MAX_UAV; ++binding)
				{
					const VulkanTextureView* texture = m_UAVTextures[set][binding];
					if (texture != nullptr)
					{
						m_DescriptorUpdater.SetStorageImage(binding, texture->VulkanImageView());
						continue;
					}

					const VulkanBufferView* buffer = m_UAVBuffers[set][binding];
					if (buffer != nullptr)
					{
						if (buffer->GetDesc().m_Format == PF_NONE)
						{
							VkBuffer vulkanBuffer = static_cast<const VulkanBuffer&>(buffer->GetBuffer()).GetVulkanBuffer();
							VkDeviceSize offset = buffer->GetDesc().m_Offset;
							VkDeviceSize range = buffer->GetDesc().m_Range;
							m_DescriptorUpdater.SetStorageBuffer(binding, vulkanBuffer, offset, range);
						}
						else
						{
							m_DescriptorUpdater.SetStorageTexelBuffer(binding, buffer->GetVulkanBufferView());
						}
					}
				}
			}
		}

		m_DescriptorUpdater.Update();

		VkPipelineBindPoint bindPoint = m_CommandType == CommandType::Draw ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;
		vkCmdBindDescriptorSets(m_CommandBuffer, bindPoint, m_PipelineState->m_PipelineLayout, m_FirstDirtySet, ResourceBindFrequency_Count - m_FirstDirtySet, m_DescriptorSets + m_FirstDirtySet, 0, nullptr);

		m_FirstDirtySet = ResourceBindFrequency_Count;
	}

	inline void RenderContext::BindPipeline()
	{
		if (m_PipelineDirty)
		{
			VkPipelineBindPoint bindPoint = m_CommandType == CommandType::Draw ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;
			vkCmdBindPipeline(m_CommandBuffer, bindPoint, m_PipelineState->m_Pipeline);
			m_FirstDirtySet = 0;
		}
	}

	inline void RenderContext::BindDynamicStates()
	{
		if (m_ViewportDirty)
		{
			vkCmdSetViewport(m_CommandBuffer, 0, 1, &m_Viewport);
			m_ViewportDirty = false;
		}
		if (m_ScissorDirty)
		{
			vkCmdSetScissor(m_CommandBuffer, 0, 1, &m_Scissor);
			m_ScissorDirty = false;
		}
	}
}

#endif // KH_GFXAPI_VULKAN