#include "engine/precomp.h"
#include "engine/mainloop.h"
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
			Khan::RenderBackend::AdvanceFrame();
		}
	}
}