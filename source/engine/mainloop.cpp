#include "engine/precomp.h"
#include "engine/mainloop.h"
#include "system/window.hpp"
#include "graphics/graphicsmanager.hpp"
#include <chrono>
#include <string>

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
			GraphicsManager::Get()->Render();
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - start;
			timer += elapsed_seconds.count();
			if (timer > 1.0)
			{
				OutputDebugString((std::to_string(frames) + "\n").c_str());
				timer = 0.0;
				frames = 0;
			}
			frames++;
		}
	}
}