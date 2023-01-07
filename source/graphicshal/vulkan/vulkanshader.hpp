#pragma once

#ifdef KH_GFXAPI_VULKAN

#include "graphicshal/shader.hpp"
#include "graphicshal/vulkan/spirv/spirv_reflect.h"

namespace Khan
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const VkPipelineShaderStageCreateInfo& shaderInfo, const SpvReflectShaderModule& reflection, const char* entryPoint, ShaderType type);
		virtual ~VulkanShader() override = default;

		inline const VkPipelineShaderStageCreateInfo& GetShaderInfo() const { return m_ShaderInfo; }
		inline VkPipelineShaderStageCreateInfo& GetShaderInfo() { return m_ShaderInfo; }
		inline const SpvReflectShaderModule& GetReflection() const { return m_Reflection; }
		inline SpvReflectShaderModule& GetReflection() { return m_Reflection; }

	private:
		VkPipelineShaderStageCreateInfo m_ShaderInfo;
		SpvReflectShaderModule m_Reflection;
		std::string m_EntryPoint;
	};
}

#endif // KH_GFXAPI_VULKAN