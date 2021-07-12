#include "engine/precomp.h"
#include "engine/mainloop.h"
#include "engine/engine.h"
#include "system/window.hpp"
#include "graphics/hal/renderbackend.hpp"
#include <chrono>

namespace Khan
{
	void MainLoop::Run()
	{
		m_Running = true;
		m_UpdateTimer.Start();
		uint64_t frames = 0;
		double timer = 0;
		while (Window::IsRunning())
		{
			auto start = std::chrono::steady_clock::now();
			Window::HandleEvents();
			Engine::g_Renderer->PreRender();
			Engine::g_Renderer->Render();
			Engine::g_Renderer->PostRender();
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			timer += elapsed_seconds.count();
			if (timer > 1.0)
			{
				timer = 0.0;
				frames = 0;
			}
			frames++;
		}
	}
}