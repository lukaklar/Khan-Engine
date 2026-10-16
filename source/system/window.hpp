#pragma once

#include "core/primitivetypes.h"

namespace Khan
{
	namespace Window
	{
		void Initialize(const char* title, uint32 width, uint32 height);
		void Shutdown();
		void HandleEvents();
		bool IsRunning();

		extern HWND g_hWnd;
		extern uint32 g_Width;
		extern uint32 g_Height;
		extern const char* g_Title;
	}
}