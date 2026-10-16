#pragma once

#include "core/primitivetypes.h"

namespace Khan
{
	class SplashScreen
	{
	public:
		SplashScreen(const char* programName = "Khan Engine", const char* imageName = "splash.bmp", uint32 width = 400, uint32 height = 300);
		~SplashScreen();

		void Update();

	private:
		LRESULT HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK WindowProcSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK WindowProcRedirect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		static const char* ms_WindowClassName;

		HINSTANCE m_hInstance;
		HWND m_hWnd;
		HBITMAP m_hBitmap;
		const char* m_ImageName;
		uint32 m_Width, m_Height;
	};
}