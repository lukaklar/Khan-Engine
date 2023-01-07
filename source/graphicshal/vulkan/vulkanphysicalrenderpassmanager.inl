#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkanutils.hpp"
#include "graphicshal/vulkan/vulkancore.h"

namespace Khan
{
	KH_FORCE_INLINE VulkanPhysicalRenderPass* VulkanPhysicalRenderPassManager::FindOrCreatePhysicalRenderPass(VkDevice device, const PhysicalRenderPassDescription& desc)
	{
		auto it = m_PhysicalRenderPassCache.find(desc);
		if (it != m_PhysicalRenderPassCache.end())
		{
			return it->second;
		}
		return CreatePhysicalRenderPass(device, desc);
	}

	KH_FORCE_INLINE VulkanPhysicalRenderPass* VulkanPhysicalRenderPassManager::CreatePhysicalRenderPass(VkDevice device, const PhysicalRenderPassDescription& desc)
	{
		static const VkAttachmentLoadOp s_VkLoadOps[] =
		{
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VK_ATTACHMENT_LOAD_OP_LOAD,
			VK_ATTACHMENT_LOAD_OP_CLEAR
		};

		static const VkAttachmentStoreOp s_VkStoreOps[] =
		{
			VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VK_ATTACHMENT_STORE_OP_STORE
		};

		

		VkAttachmentDescription attachments[K_MAX_RENDER_TARGETS + 1];
		VkAttachmentReference colorAttachmentReferences[K_MAX_RENDER_TARGETS];
		VkAttachmentReference depthStencilAttachmentReference;
		uint32_t attachmentCount = desc.m_RenderTargetCount;
		bool depthStencilUsed = desc.m_DepthStencil.m_Format != PF_NONE;
		bool dependencyNecessary = false;

		for (uint32_t i = 0; i < desc.m_RenderTargetCount; ++i)
		{
			VkAttachmentDescription& attachment = attachments[i];
			attachment.flags = 0;
			attachment.format = PixelFormatToVulkanFormat(desc.m_RenderTargets[i].m_Format);
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = s_VkLoadOps[static_cast<uint32_t>(desc.m_RenderTargets[i].m_StartAccess)];
			attachment.storeOp = s_VkStoreOps[static_cast<uint32_t>(desc.m_RenderTargets[i].m_EndAccess)];
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachment.initialLayout = desc.m_RenderTargets[i].m_StartAccess == StartAccessType::Keep ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			colorAttachmentReferences[i].attachment = i;
			colorAttachmentReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			dependencyNecessary = dependencyNecessary || desc.m_RenderTargets[i].m_StartAccess == StartAccessType::Clear;
		}

		if (depthStencilUsed)
		{
			++attachmentCount;
			bool depthOrStencilLoaded = desc.m_DepthStencil.m_DepthStartAccess == StartAccessType::Keep || desc.m_DepthStencil.m_StencilStartAccess == StartAccessType::Keep;
			VkAttachmentDescription& attachment = attachments[desc.m_RenderTargetCount];
			attachment.flags = 0;
			attachment.format = PixelFormatToVulkanFormat(desc.m_DepthStencil.m_Format);
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = s_VkLoadOps[static_cast<uint32_t>(desc.m_DepthStencil.m_DepthStartAccess)];
			attachment.storeOp = s_VkStoreOps[static_cast<uint32_t>(desc.m_DepthStencil.m_DepthEndAccess)];
			attachment.stencilLoadOp = s_VkLoadOps[static_cast<uint32_t>(desc.m_DepthStencil.m_StencilStartAccess)];
			attachment.stencilStoreOp = s_VkStoreOps[static_cast<uint32_t>(desc.m_DepthStencil.m_StencilEndAccess)];
			attachment.initialLayout = depthOrStencilLoaded ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			dependencyNecessary = dependencyNecessary || desc.m_DepthStencil.m_DepthStartAccess == StartAccessType::Clear || desc.m_DepthStencil.m_StencilStartAccess == StartAccessType::Clear;

			depthStencilAttachmentReference.attachment = desc.m_RenderTargetCount;
			depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDependency dependency;
		if (dependencyNecessary)
		{
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | (depthStencilUsed ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT : 0);
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | (depthStencilUsed ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0);
			dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = desc.m_RenderTargetCount;
		subpass.pColorAttachments = colorAttachmentReferences;
		subpass.pDepthStencilAttachment = depthStencilUsed ? &depthStencilAttachmentReference : nullptr;

		VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		renderPassInfo.attachmentCount = attachmentCount;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = dependencyNecessary ? 1 : 0;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass renderPass;
		VK_ASSERT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass), "[VULKAN] Failed to create render pass.");

		VulkanPhysicalRenderPass* physicalRenderPass = new VulkanPhysicalRenderPass(renderPass, desc);
		m_PhysicalRenderPassCache.emplace(desc, physicalRenderPass);

		return physicalRenderPass;
	}
}

#endif // KH_GFXAPI_VULKAN