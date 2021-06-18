#pragma once

namespace Khan
{
	namespace Window
	{
		void Initialize(const char* title, uint32_t width, uint32_t height);
		void Shutdown();
		void HandleEvents();
		bool IsRunning();

		extern HWND g_hWnd;
		extern uint32_t g_Width;
		extern uint32_t g_Height;
		extern const char* g_Title;
	}
}