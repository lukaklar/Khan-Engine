#include "graphics/precomp.h"
#include "graphics/graphicsmanager.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/renderer.hpp"
#include "graphics/shadermanager.hpp"

namespace Khan
{
	GraphicsManager::GraphicsManager()
	{
		RenderBackend::Initialize(true);
		ShaderManager::CreateSingleton();
		m_Renderer = new Renderer();
	}

	GraphicsManager::~GraphicsManager()
	{
		delete m_Renderer;
		ShaderManager::DestroySingleton();
		RenderBackend::Shutdown();
	}

	void GraphicsManager::Render()
	{
		m_Renderer->PreRender();
		m_Renderer->Render();
		m_Renderer->PostRender();
	}
}