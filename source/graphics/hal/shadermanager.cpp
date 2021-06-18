#include "graphics/precomp.h"
#include "graphics/hal/shadermanager.hpp"

namespace Khan
{
	ShaderManager& ShaderManager::Get()
	{
		static ShaderManager s_Instance;
		return s_Instance;
	}

	ShaderManager::~ShaderManager()
	{
		for (auto map : m_ShaderMaps)
		{
			for (auto it : map)
			{
				RenderBackend::g_Device->DestroyShader(it.second);
			}
		}
	}
}