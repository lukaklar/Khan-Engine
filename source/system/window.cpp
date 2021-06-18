#include "system/precomp.h"
#include "system/window.hpp"

namespace Khan
{
	namespace Window
	{
		static bool s_Running;
		static HINSTANCE s_hInstance;
		HWND g_hWnd;
		uint32_t g_Width;
		uint32_t g_Height;
		const char* g_Title;

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			switch (message)
			{
			case WM_DESTROY:
			case WM_CLOSE:
			{
				s_Running = false;
				break;
			}
			default:
			{
				return DefWindowProc(hwnd, message, wparam, lparam);
			}
			}
			return 0;
		}

		void Initialize(const char* title, uint32_t width, uint32_t height)
		{
			g_Title = title;
			g_Width = width;
			g_Height = height;
			s_Running = true;

			WNDCLASSEX wcx = {};
			wcx.cbSize = sizeof(wcx);
			wcx.style = CS_VREDRAW | CS_HREDRAW;
			wcx.lpfnWndProc = WindowProc;
			wcx.hInstance = GetModuleHandle(NULL);
			wcx.lpszClassName = title;

			RegisterClassEx(&wcx);

			int screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
			int screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;

			RECT rc;
			rc.left = screenCenterX - g_Width / 2;
			rc.top = screenCenterY - g_Height / 2;
			rc.right = rc.left + g_Width;
			rc.bottom = rc.top + g_Height;

			AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

			g_hWnd = CreateWindowEx(0, title, title, WS_OVERLAPPEDWINDOW, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, HWND_DESKTOP, NULL, GetModuleHandle(NULL), NULL);

			ShowWindow(g_hWnd, SW_SHOWDEFAULT);
			SetForegroundWindow(g_hWnd);
			SetFocus(g_hWnd);
		}

		void Shutdown()
		{
			DestroyWindow(g_hWnd);
			UnregisterClass(g_Title, s_hInstance);
		}

		void HandleEvents()
		{
			MSG msg;
			//while (s_Running)
			//{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			//}
		}

		bool IsRunning()
		{
			return s_Running;
		}
	}
}