#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "core/containers.h"
#include "graphicshal/physicalrenderpass.hpp"

namespace Khan
{
	class VulkanPhysicalRenderPass : public PhysicalRenderPass
	{
	public:
		VulkanPhysicalRenderPass(VkRenderPass renderPass, const PhysicalRenderPassDescription& desc)
			: PhysicalRenderPass(desc)
			, m_RenderPass(renderPass)
		{
		}

		VkRenderPass VulkanRenderPass() const { return m_RenderPass; }

	private:
		VkRenderPass m_RenderPass;
	};

	class VulkanPhysicalRenderPassManager
	{
	public:
		//void Create(VkDevice device);
		void Destroy(VkDevice device);

		VulkanPhysicalRenderPass* FindOrCreatePhysicalRenderPass(VkDevice device, const PhysicalRenderPassDescription& desc);

	private:
		VulkanPhysicalRenderPass* CreatePhysicalRenderPass(VkDevice device, const PhysicalRenderPassDescription& desc);

		THashMap<PhysicalRenderPassDescription, VulkanPhysicalRenderPass*> m_PhysicalRenderPassCache;
	};
}

#include "graphicshal/vulkan/vulkanphysicalrenderpassmanager.inl"

#endif // KH_GFXAPI_VULKAN