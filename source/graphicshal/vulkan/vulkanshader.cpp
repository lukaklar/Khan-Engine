#include "graphicshal/precomp.h"

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/vulkan/vulkanshader.hpp"

namespace Khan
{
	VulkanShader::VulkanShader(const VkPipelineShaderStageCreateInfo& shaderInfo, const SpvReflectShaderModule& reflection, const char* entryPoint, ShaderType type)
		: Shader(type)
		, m_ShaderInfo(shaderInfo)
		, m_Reflection(reflection)
		, m_EntryPoint(entryPoint)
	{
		m_ShaderInfo.pName = m_EntryPoint.c_str();
	}
}

#endif // KH_GFXAPI_VULKAN