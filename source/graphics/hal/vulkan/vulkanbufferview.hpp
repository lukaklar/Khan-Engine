#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/bufferview.hpp"
#include <vulkan/vulkan.h>

namespace Khan
{
	class VulkanBufferView : public BufferView
	{
	public:
		VulkanBufferView(VkBufferView view, Buffer& buffer, const BufferViewDesc& desc)
			: BufferView(buffer, desc)
			, m_BufferView(view)
		{
		}

		inline const VkBufferView& GetVulkanBufferView() const { return m_BufferView; }

	protected:
		friend class VulkanTransientResourceManager;
		VulkanBufferView() = default;

	private:
		VkBufferView m_BufferView;
	};
}

#endif // KH_GFXAPI_VULKAN