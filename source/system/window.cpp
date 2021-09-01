#include "system/precomp.h"
#include "system/window.hpp"
#include "system/input/inputmanager.hpp"

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

		static void HideCursor()
		{
			while (::ShowCursor(FALSE) >= 0);
		}

		static void ShowCursor()
		{
			while (::ShowCursor(TRUE) < 0);
		}

		static void ConfineCursor()
		{
			RECT rect;
			GetClientRect(g_hWnd, &rect);
			MapWindowPoints(g_hWnd, NULL, reinterpret_cast<POINT*>(&rect), 2);
			ClipCursor(&rect);
		}

		static void FreeCursor()
		{
			ClipCursor(NULL);
		}

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			switch (message)
			{
			case WM_INPUT:
			{
				std::vector<uint8_t> rawBuffer;
				UINT size;

				if (FAILED(GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER))))
				{
					break;
				}

				rawBuffer.resize(size);

				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
				{
					break;
				}

				auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
				if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
				{
					InputManager::Get()->OnRawCursorMove(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
				}

				break;
			}
			case WM_MOUSEMOVE:
			{
				InputManager::Get()->OnCursorMoved(LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_KEYDOWN:
			{
				bool prevState = (lparam & (1 << 30)) != 0;
				InputManager::Get()->OnKeyPressed(static_cast<int32_t>(wparam), LOWORD(lparam), prevState ? InputType::Repeat : InputType::Press);
				break;
			}
			case WM_KEYUP:
			{
				InputManager::Get()->OnKeyPressed(static_cast<int32_t>(wparam), LOWORD(lparam), InputType::Release);
				break;
			}
			case WM_LBUTTONDOWN:
			{
				//ConfineCursor();
				//HideCursor();
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Left, ClickType::Single, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_LBUTTONUP:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Left, ClickType::Release, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_LBUTTONDBLCLK:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Left, ClickType::Double, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_RBUTTONDOWN:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Right, ClickType::Single, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_RBUTTONUP:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Right, ClickType::Release, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_RBUTTONDBLCLK:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Right, ClickType::Double, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_MBUTTONDOWN:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Middle, ClickType::Single, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_MBUTTONUP:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Middle, ClickType::Release, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_MBUTTONDBLCLK:
			{
				InputManager::Get()->OnMouseButtonPressed(MouseButton::Middle, ClickType::Double, LOWORD(lparam), HIWORD(lparam));
				break;
			}
			case WM_ACTIVATE:
			{
				if (wparam & (WA_ACTIVE | WA_CLICKACTIVE))
				{
					ConfineCursor();
					HideCursor();
				}
				else
				{
					FreeCursor();
					ShowCursor();
				}
				break;
			}
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

			RAWINPUTDEVICE rid;
			rid.usUsagePage = 0x01;
			rid.usUsage = 0x02;
			rid.dwFlags = 0;
			rid.hwndTarget = g_hWnd;
			RegisterRawInputDevices(&rid, 1, sizeof(rid));
		}

		void Shutdown()
		{
			DestroyWindow(g_hWnd);
			UnregisterClass(g_Title, s_hInstance);
		}

		void HandleEvents()
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		bool IsRunning()
		{
			return s_Running;
		}
	}
}