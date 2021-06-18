#pragma once

namespace Khan
{
	class SplashScreen
	{
	public:
		SplashScreen(const char* programName = "Khan Engine", const char* imageName = "splash.bmp", uint32_t width = 400, uint32_t height = 300);
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
		uint32_t m_Width, m_Height;
	};
}