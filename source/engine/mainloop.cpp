#include "engine/precomp.h"
#include "engine/mainloop.h"
#include "engine/systems/systemmanager.hpp"
#include "graphics/graphicsmanager.hpp"
#include "system/window.hpp"
#include <chrono>
#include <string>

namespace Khan
{
	void MainLoop::Run()
	{
		m_Running = true;
		m_UpdateTimer.Start();
		uint64_t frames = 0;
		float timer = 0.0f;
		float dt = 0.0f;
		while (Window::IsRunning())
		{
			auto start = std::chrono::steady_clock::now();
			Window::HandleEvents();
			SystemManager::Get()->UpdateSystems(dt);
			GraphicsManager::Get()->Render();
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsed_seconds = end - start;
			dt = elapsed_seconds.count();
			timer += dt;
			if (timer > 1.0f)
			{
				OutputDebugString((std::to_string(frames) + "\n").c_str());
				timer = 0.0f;
				frames = 0;
			}
			frames++;
		}
	}
}