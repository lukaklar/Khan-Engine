#include "graphics/precomp.h"
#include "graphics/shadermanager.hpp"

namespace Khan
{
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