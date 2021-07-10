#include "engine/precomp.h"
#include "engine/mainloop.h"
#include "engine/engine.h"
#include "system/window.hpp"
#include "graphics/hal/renderbackend.hpp"

namespace Khan
{
	void MainLoop::Run()
	{
		m_Running = true;
		m_UpdateTimer.Start();

		while (Window::IsRunning())
		{
			Window::HandleEvents();
			Engine::g_Renderer->PreRender();
			Engine::g_Renderer->Render();
			Engine::g_Renderer->PostRender();
			//Khan::RenderBackend::AdvanceFrame();
		}
	}
}