#pragma once
#include "graphics/hal/shader.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "core/singleton.h"

#ifdef KH_GFXAPI_VULKAN
#include "graphics/hal/vulkan/vulkandevice.hpp"
#endif // KH_GFXAPI_VULKAN

#include <fstream>

namespace Khan
{
	class ShaderManager : public Singleton<ShaderManager>
	{
	public:
		~ShaderManager();

		template<ShaderType shaderType>
		const Shader* GetShader(const char* name, const char* entryPoint)
		{
			std::string fullFileName = ms_Path;
			fullFileName += name;
			fullFileName += ms_Extension;

			std::string fullName(fullFileName + "|" + entryPoint);

			auto shaderMapIt = m_ShaderMaps[shaderType].find(fullName);
			if (shaderMapIt != m_ShaderMaps[shaderType].end())
			{
				return shaderMapIt->second;
			}

			auto shaderFileMapIt = m_ShaderFileMap.find(fullFileName);
			if (shaderFileMapIt == m_ShaderFileMap.end())
			{
				std::ifstream file(fullFileName, std::ios::ate | std::ios::binary);

				if (!file.is_open()) {
					throw std::runtime_error("failed to open file!");
				}

				size_t fileSize = (size_t)file.tellg();
				std::vector<uint8_t> shaderFileData(fileSize);

				file.seekg(0);
				file.read((char*)shaderFileData.data(), fileSize);

				file.close();

				shaderFileMapIt = m_ShaderFileMap.emplace(name, std::move(shaderFileData)).first;
			}

			ShaderDesc desc;
			desc.m_Type = shaderType;
			desc.m_Bytecode = shaderFileMapIt->second.data();
			desc.m_BytecodeSize = static_cast<uint32_t>(shaderFileMapIt->second.size());
			desc.m_EntryPoint = entryPoint;

			Shader* compiledShader = RenderBackend::g_Device->CreateShader(desc);
			m_ShaderMaps[shaderType].emplace(fullName, compiledShader);

			return compiledShader;
		}

	private:
		std::unordered_map<std::string, Shader*> m_ShaderMaps[ShaderType_Count];
		std::unordered_map<std::string, std::vector<uint8_t>> m_ShaderFileMap;

#ifdef KH_GFXAPI_VULKAN
		inline static constexpr char* ms_Path = "..\\..\\bin\\shaders\\vulkan\\";
		inline static constexpr char* ms_Extension = ".spv";
#endif // KH_GFXAPI_VULKAN
	};
}