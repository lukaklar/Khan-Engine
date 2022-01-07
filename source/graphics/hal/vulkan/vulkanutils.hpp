#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphics/hal/vulkan/vulkanbuffer.hpp"
#include "graphics/hal/vulkan/vulkanbufferview.hpp"
#include "graphics/hal/vulkan/vulkanshader.hpp"
#include "graphics/hal/vulkan/vulkantexture.hpp"
#include "graphics/hal/vulkan/vulkantextureview.hpp"
#include <vulkan/vulkan.h>

namespace Khan
{
	KH_FORCE_INLINE bool CheckLayer(const char* layer, const std::vector<VkLayerProperties>& presentLayers)
	{
		for (auto& properties : presentLayers)
		{
			if (!std::strcmp(layer, properties.layerName))
			{
				return true;
			}
		}
		return false;
	}

	KH_FORCE_INLINE bool CheckExtension(const char* extension, const std::vector<VkExtensionProperties>& presentExtensions)
	{
		for (auto& properties : presentExtensions)
		{
			if (!std::strcmp(extension, properties.extensionName))
			{
				return true;
			}
		}
		return false;
	}

	KH_FORCE_INLINE VkBufferUsageFlags BufferFlagsToVulkanBufferUsageFlags(BufferFlags flags)
	{
		VkBufferUsageFlags vulkanFlags = 0;

		if (flags & BufferFlag_Readable) vulkanFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if (flags & BufferFlag_Writable) vulkanFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if (flags & BufferFlag_AllowShaderResource) vulkanFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
		if (flags & BufferFlag_AllowUnorderedAccess) vulkanFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		if (flags & BufferFlag_AllowIndirect) vulkanFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		if (flags & BufferFlag_AllowVertices) vulkanFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if (flags & BufferFlag_AllowIndices) vulkanFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		return vulkanFlags;
	}

	KH_FORCE_INLINE VkImageType TextureTypeToVulkanImageType(TextureType type)
	{
		static const VkImageType s_VkImageType[] =
		{
			VK_IMAGE_TYPE_1D,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_TYPE_3D
		};

		return s_VkImageType[type];
	}

	KH_FORCE_INLINE VkFormat PixelFormatToVulkanFormat(PixelFormat format)
	{
		static const VkFormat s_VkFormats[] =
		{
			VK_FORMAT_UNDEFINED,				// PF_NONE
			VK_FORMAT_D16_UNORM,				// PF_D16_UNORM
			VK_FORMAT_D24_UNORM_S8_UINT,		// PF_D24_UNORM_S8_UINT
			VK_FORMAT_D32_SFLOAT,				// PF_D32_FLOAT
			VK_FORMAT_D32_SFLOAT_S8_UINT,		// PF_D32_FLOAT_S8_UINT
			VK_FORMAT_R8G8B8A8_UNORM,			// PF_R8G8B8A8_UNORM
			VK_FORMAT_B8G8R8A8_UNORM,			// PF_B8G8R8A8_UNORM
			VK_FORMAT_R16G16_SFLOAT,			// PF_R16G16_FLOAT
			VK_FORMAT_B10G11R11_UFLOAT_PACK32,	// PF_R11G11B10_FLOAT
			VK_FORMAT_R8G8B8A8_SRGB,			// PF_R8G8B8A8_SRGB
			VK_FORMAT_R16G16B16A16_SFLOAT,		// PF_R16G16B16A16_FLOAT
			VK_FORMAT_A2B10G10R10_UNORM_PACK32,	// PF_R10G10B10A2_UNORM
			VK_FORMAT_R32_SFLOAT,				// PF_R32_FLOAT
			VK_FORMAT_R32_UINT,					// PF_R32_UINT
			VK_FORMAT_R32G32_UINT				// PF_R32G32_UINT
		};

		return s_VkFormats[format];
	}

	KH_FORCE_INLINE VkImageAspectFlags PixelFormatToVulkanAspectMask(PixelFormat format)
	{
		static const VkImageAspectFlags s_VkImageAspectFlags[] =
		{
			0,															// PF_NONE
			VK_IMAGE_ASPECT_DEPTH_BIT,									// PF_D16_UNORM
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,	// PF_D24_UNORM_S8_UINT
			VK_IMAGE_ASPECT_DEPTH_BIT,									// PF_D32_FLOAT
			VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,	// PF_D32_FLOAT_S8_UINT
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R8G8B8A8_UNORM
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_B8G8R8A8_UNORM
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R16G16_FLOAT
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R11G11B10_FLOAT
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R8G8B8A8_SRGB
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R16G16B16A16_FLOAT
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R10G10B10A2_UNORM
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R32_FLOAT
			VK_IMAGE_ASPECT_COLOR_BIT,									// PF_R32_UINT
			VK_IMAGE_ASPECT_COLOR_BIT									// PF_R32G32_UINT
		};

		return s_VkImageAspectFlags[format];
	}

	KH_FORCE_INLINE VkImageUsageFlags TextureFlagsToVulkanImageUsageFlags(TextureFlags flags)
	{
		VkImageUsageFlags vulkanFlags = 0;

		if (flags & TextureFlag_Readable) vulkanFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (flags & TextureFlag_Writable) vulkanFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (flags & TextureFlag_AllowShaderResource) vulkanFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
		if (flags & TextureFlag_AllowUnorderedAccess) vulkanFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
		if (flags & TextureFlag_AllowRenderTarget) vulkanFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (flags & TextureFlag_AllowDepthStencil) vulkanFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		return vulkanFlags;
	}

	KH_FORCE_INLINE VkImageViewType TextureViewTypeToVulkanImageViewType(TextureViewType type)
	{
		static const VkImageViewType s_VkImageViewTypes[] =
		{
			VK_IMAGE_VIEW_TYPE_1D,
			VK_IMAGE_VIEW_TYPE_2D,
			VK_IMAGE_VIEW_TYPE_3D,
			VK_IMAGE_VIEW_TYPE_CUBE,
			VK_IMAGE_VIEW_TYPE_1D_ARRAY,
			VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
		};

		return s_VkImageViewTypes[type];
	}

	KH_FORCE_INLINE VkShaderStageFlagBits ShaderTypeToVulkanShaderStage(ShaderType type)
	{
		static const VkShaderStageFlagBits s_VkShaderStages[] =
		{
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
			VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
			VK_SHADER_STAGE_GEOMETRY_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			VK_SHADER_STAGE_COMPUTE_BIT
		};

		return s_VkShaderStages[type];
	}
}

#endif // KH_GFXAPI_VULKAN