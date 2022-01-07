#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/resourcestate.hpp"
#include "graphics/hal/vulkan/vulkanadapter.hpp"
#include "graphics/hal/vulkan/vulkanutils.hpp"

namespace Khan
{
	KH_FORCE_INLINE static VkAccessFlags ResourceStateToVkAccess(ResourceState state)
	{
		static const VkAccessFlags s_VkAccessFlags[] =
		{
			0,																								// ResourceState_Undefined
			0,																								// ResourceState_General
			VK_ACCESS_TRANSFER_READ_BIT,																	// ResourceState_CopySource
			VK_ACCESS_TRANSFER_WRITE_BIT,																	// ResourceState_CopyDestination
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,															// ResourceState_VertexBuffer
			VK_ACCESS_INDEX_READ_BIT,																		// ResourceState_IndexBuffer
			VK_ACCESS_INDIRECT_COMMAND_READ_BIT,															// ResourceState_IndirectArgument
			VK_ACCESS_UNIFORM_READ_BIT,																		// ResourceState_ConstantBuffer
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,											// ResourceState_UnorderedAccess
			VK_ACCESS_SHADER_READ_BIT,																		// ResourceState_NonPixelShaderAccess
			VK_ACCESS_SHADER_READ_BIT,																		// ResourceState_PixelShaderAccess
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,											// ResourceState_PixelShaderWrite
			VK_ACCESS_SHADER_READ_BIT,																		// ResourceState_AnyShaderAccess
			VK_ACCESS_SHADER_WRITE_BIT,																		// ResourceState_StreamOut
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,						// ResourceState_RenderTarget
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,		// ResourceState_DepthWrite
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,													// ResourceState_DepthRead
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,		// ResourceState_StencilWrite
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,													// ResourceState_StencilRead
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,		// ResourceState_DepthWriteStencilWrite
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,													// ResourceState_DepthReadStencilRead
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,		// ResourceState_DepthReadStencilWrite
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,		// ResourceState_DepthWriteStencilRead
			VK_ACCESS_MEMORY_READ_BIT																		// ResourceState_Present
		};

		return s_VkAccessFlags[state];
	}

	KH_FORCE_INLINE static VkImageLayout ResourceStateToVkImageLayout(ResourceState state)
	{
		static const VkImageLayout s_VkImageLayout[] =
		{
			VK_IMAGE_LAYOUT_UNDEFINED,											// ResourceState_Undefined
			VK_IMAGE_LAYOUT_GENERAL,											// ResourceState_General
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,								// ResourceState_CopySource
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,								// ResourceState_CopyDestination
			VK_IMAGE_LAYOUT_UNDEFINED,											// ResourceState_VertexBuffer
			VK_IMAGE_LAYOUT_UNDEFINED,											// ResourceState_IndexBuffer
			VK_IMAGE_LAYOUT_UNDEFINED,											// ResourceState_IndirectArgument
			VK_IMAGE_LAYOUT_UNDEFINED,											// ResourceState_ConstantBuffer
			VK_IMAGE_LAYOUT_GENERAL,											// ResourceState_UnorderedAccess
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,							// ResourceState_NonPixelShaderAccess
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,							// ResourceState_PixelShaderAccess
			VK_IMAGE_LAYOUT_GENERAL,											// ResourceState_PixelShaderWrite
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,							// ResourceState_AnyShaderAccess
			VK_IMAGE_LAYOUT_UNDEFINED,											// ResourceState_StreamOut
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,							// ResourceState_RenderTarget
			VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR,						// ResourceState_DepthWrite
			VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL_KHR,						// ResourceState_DepthRead
			VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL_KHR,						// ResourceState_StencilWrite
			VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL_KHR,						// ResourceState_StencilRead
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,					// ResourceState_DepthWriteStencilWrite
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,					// ResourceState_DepthReadStencilRead
			VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR,		// ResourceState_DepthReadStencilWrite
			VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR,		// ResourceState_DepthWriteStencilRead
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR										// ResourceState_Present
		};

		return s_VkImageLayout[state];
	}

	KH_FORCE_INLINE static VkPipelineStageFlags ResourceStateToVkPipelineStageFlags(ResourceState state)
	{
		static constexpr VkPipelineStageFlags s_AllShaderStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
																| VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
																| VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
																| VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
																| VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
																| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		static constexpr VkPipelineStageFlags s_AllExceptFragmentShaderStages = s_AllShaderStages & ~VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		static const VkPipelineStageFlags s_VkPipelineStageFlags[] =
		{
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,														// ResourceState_Undefined
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,														// ResourceState_General
			VK_PIPELINE_STAGE_TRANSFER_BIT,															// ResourceState_CopySource
			VK_PIPELINE_STAGE_TRANSFER_BIT,															// ResourceState_CopyDestination
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,														// ResourceState_VertexBuffer
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,														// ResourceState_IndexBuffer
			VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,													// ResourceState_IndirectArgument
			s_AllShaderStages,																		// ResourceState_ConstantBuffer
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,													// ResourceState_UnorderedAccess
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,													// ResourceState_NonPixelShaderAccess
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,													// ResourceState_PixelShaderAccess
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,													// ResourceState_PixelShaderWrite
			s_AllShaderStages,																		// ResourceState_AnyShaderAccess
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,														// ResourceState_StreamOut
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,											// ResourceState_RenderTarget
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_DepthWrite
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_DepthRead
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_StencilWrite
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_StencilRead
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_DepthWriteStencilWrite
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_DepthReadStencilRead
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_DepthReadStencilWrite
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,	// ResourceState_DepthWriteStencilRead
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT														// ResourceState_Present
		};

		return s_VkPipelineStageFlags[state];
	}

	KH_FORCE_INLINE static bool ResourceStateToWriteAccess(ResourceState state)
	{
		return state == ResourceState_CopyDestination || state == ResourceState_UnorderedAccess || state == ResourceState_PixelShaderWrite;
	}

	KH_FORCE_INLINE void VulkanBarrierRecorder::RecordBarrier(VulkanBuffer& buffer, ResourceState state, QueueType queue)
	{
		if (buffer.GetQueue() == QueueType_None)
		{
			buffer.SetQueue(queue);
		}

		if (!ResourceStateToWriteAccess(buffer.GetState()) && !ResourceStateToWriteAccess(state) && queue == buffer.GetQueue())
		{
			return;
		}

		VkBufferMemoryBarrier& barrier = m_BufferBarriers[m_BufferBarrierCount++];

		barrier.srcAccessMask = ResourceStateToVkAccess(buffer.GetState());
		barrier.dstAccessMask = ResourceStateToVkAccess(state);
		
		if (queue != buffer.GetQueue())
		{
			barrier.srcQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[buffer.GetQueue()];
			barrier.dstQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[queue];
			buffer.SetQueue(queue);
		}
		else
		{
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		}

		barrier.buffer = buffer.GetVulkanBuffer();
		barrier.offset = 0;
		barrier.size = VK_WHOLE_SIZE;

		m_SrcStages |= ResourceStateToVkPipelineStageFlags(buffer.GetState());
		m_DstStages |= ResourceStateToVkPipelineStageFlags(state);

		buffer.SetState(state);
	}

	KH_FORCE_INLINE void VulkanBarrierRecorder::RecordBarrier(VulkanBufferView& bufferView, ResourceState state, QueueType queue, bool memoryBarrier)
	{
		if (bufferView.GetBuffer().GetQueue() == QueueType_None)
		{
			bufferView.GetBuffer().SetQueue(queue);
		}

		if (!ResourceStateToWriteAccess(bufferView.GetBuffer().GetState()) && !ResourceStateToWriteAccess(state) && queue == bufferView.GetBuffer().GetQueue())
		{
			return;
		}

		if (memoryBarrier)
		{
			VkMemoryBarrier& barrier = m_MemoryBarriers[m_MemoryBarrierCount++];

			barrier.srcAccessMask = ResourceStateToVkAccess(bufferView.GetBuffer().GetState());
			barrier.dstAccessMask = ResourceStateToVkAccess(state);
		}
		else
		{
			VkBufferMemoryBarrier& barrier = m_BufferBarriers[m_BufferBarrierCount++];

			barrier.srcAccessMask = ResourceStateToVkAccess(bufferView.GetBuffer().GetState());
			barrier.dstAccessMask = ResourceStateToVkAccess(state);

			if (queue != bufferView.GetBuffer().GetQueue())
			{
				barrier.srcQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[bufferView.GetBuffer().GetQueue()];
				barrier.dstQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[queue];
				bufferView.GetBuffer().SetQueue(queue);
			}
			else
			{
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			}

			barrier.buffer = static_cast<VulkanBuffer&>(bufferView.GetBuffer()).GetVulkanBuffer();
			barrier.offset = bufferView.GetDesc().m_Offset;
			barrier.size = VK_WHOLE_SIZE;
		}


		m_SrcStages |= ResourceStateToVkPipelineStageFlags(bufferView.GetBuffer().GetState());
		m_DstStages |= ResourceStateToVkPipelineStageFlags(state);

		bufferView.GetBuffer().SetState(state);
	}

	KH_FORCE_INLINE void VulkanBarrierRecorder::RecordBarrier(VulkanTexture& texture, ResourceState state, QueueType queue)
	{
		VkImageLayout oldLayout = ResourceStateToVkImageLayout(texture.GetState());
		VkImageLayout newLayout = ResourceStateToVkImageLayout(state);

		if (texture.GetQueue() == QueueType_None)
		{
			texture.SetQueue(queue);
		}

		if (!ResourceStateToWriteAccess(texture.GetState()) && !ResourceStateToWriteAccess(state) && oldLayout == newLayout && texture.GetQueue() == queue)
		{
			return;
		}

		VkImageMemoryBarrier& barrier = m_ImageBarriers[m_ImageBarrierCount++];

		barrier.srcAccessMask = ResourceStateToVkAccess(texture.GetState());
		barrier.dstAccessMask = ResourceStateToVkAccess(state);

		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		if (queue != texture.GetQueue())
		{
			barrier.srcQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[texture.GetQueue()];
			barrier.dstQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[queue];
			texture.SetQueue(queue);
		}
		else
		{
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		}

		barrier.image = texture.VulkanImage();

		barrier.subresourceRange.aspectMask = PixelFormatToVulkanAspectMask(texture.GetDesc().m_Format);
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = texture.GetDesc().m_MipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = texture.GetDesc().m_ArrayLayers;

		m_SrcStages |= ResourceStateToVkPipelineStageFlags(texture.GetState());
		m_DstStages |= ResourceStateToVkPipelineStageFlags(state);

		texture.SetState(state);
	}

	KH_FORCE_INLINE void VulkanBarrierRecorder::RecordBarrier(VulkanTextureView& textureView, ResourceState state, QueueType queue)
	{
		VkImageLayout oldLayout = ResourceStateToVkImageLayout(textureView.GetTexture().GetState());
		VkImageLayout newLayout = ResourceStateToVkImageLayout(state);

		if (textureView.GetTexture().GetQueue() == QueueType_None)
		{
			textureView.GetTexture().SetQueue(queue);
		}

		if (!ResourceStateToWriteAccess(textureView.GetTexture().GetState()) && !ResourceStateToWriteAccess(state) && oldLayout == newLayout && textureView.GetTexture().GetQueue() == queue)
		{
			return;
		}

		VkImageMemoryBarrier& barrier = m_ImageBarriers[m_ImageBarrierCount++];

		barrier.srcAccessMask = ResourceStateToVkAccess(textureView.GetTexture().GetState());
		barrier.dstAccessMask = ResourceStateToVkAccess(state);

		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		if (queue != textureView.GetTexture().GetQueue())
		{
			barrier.srcQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[textureView.GetTexture().GetQueue()];
			barrier.dstQueueFamilyIndex = m_Adapter.GetQueueFamilyIndices()[queue];
			textureView.GetTexture().SetQueue(queue);
		}
		else
		{
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		}

		barrier.image = static_cast<VulkanTexture&>(textureView.GetTexture()).VulkanImage();

		barrier.subresourceRange.aspectMask = PixelFormatToVulkanAspectMask(textureView.GetDesc().m_Format);
		barrier.subresourceRange.baseMipLevel = textureView.GetDesc().m_BaseMipLevel;
		barrier.subresourceRange.levelCount = textureView.GetDesc().m_LevelCount;
		barrier.subresourceRange.baseArrayLayer = textureView.GetDesc().m_BaseArrayLayer;
		barrier.subresourceRange.layerCount = textureView.GetDesc().m_LayerCount;

		m_SrcStages |= ResourceStateToVkPipelineStageFlags(textureView.GetTexture().GetState());
		m_DstStages |= ResourceStateToVkPipelineStageFlags(state);

		textureView.GetTexture().SetState(state);
	}

	KH_FORCE_INLINE void VulkanBarrierRecorder::Flush(VkCommandBuffer commandBuffer)
	{
		if (m_BufferBarrierCount > 0 || m_ImageBarrierCount > 0)
		{
			vkCmdPipelineBarrier(commandBuffer, m_SrcStages, m_DstStages, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, m_BufferBarrierCount, m_BufferBarriers, m_ImageBarrierCount, m_ImageBarriers);
			Reset();
		}
	}

	KH_FORCE_INLINE void VulkanBarrierRecorder::Reset()
	{
		m_ImageBarrierCount = 0;
		m_BufferBarrierCount = 0;
		m_MemoryBarrierCount = 0;
		m_SrcStages = 0;
		m_DstStages = 0;
	}
}

#endif // KH_GFXAPI_VULKAN