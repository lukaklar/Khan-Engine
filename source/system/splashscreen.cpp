#include "system/precomp.h"
#include "system/splashscreen.hpp"

namespace Khan
{
    const char* SplashScreen::ms_WindowClassName = "SplashScreen";

    SplashScreen::SplashScreen(const char* programName /* = "Khan Engine" */, const char* imageName /* = "splash.bmp" */, uint32_t width /* = 400 */, uint32_t height /* = 300 */)
        : m_hInstance(GetModuleHandle(NULL)), m_hBitmap(NULL), m_ImageName(imageName), m_Width(width), m_Height(height)
    {
        WNDCLASSEX wcx = {};
        wcx.cbSize = sizeof(WNDCLASSEX);
        wcx.lpfnWndProc = SplashScreen::WindowProcSetup;
        wcx.hInstance = m_hInstance;
        wcx.lpszClassName = ms_WindowClassName;
        wcx.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClassEx(&wcx);

        int32_t screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);

        RECT windowRect;
        windowRect.left = (screenWidth - width) / 2;
        windowRect.top = (screenHeight - height) / 2;
        windowRect.right = windowRect.left + width;
        windowRect.bottom = windowRect.top + height;

        AdjustWindowRect(&windowRect, 0, FALSE);

        m_hWnd = CreateWindowEx(0, ms_WindowClassName, programName, 0, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, HWND_DESKTOP, NULL, m_hInstance, this);

        SetWindowLong(m_hWnd, GWL_STYLE, 0);
        ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    }

    SplashScreen::~SplashScreen()
    {
        DestroyWindow(m_hWnd);
        UnregisterClass(ms_WindowClassName, m_hInstance);
    }

    void SplashScreen::Update()
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT CALLBACK SplashScreen::WindowProcSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_NCCREATE || message == WM_CREATE)
        {
            const CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            SplashScreen* pSplashScreen = reinterpret_cast<SplashScreen*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pSplashScreen));
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SplashScreen::WindowProcRedirect));
            return pSplashScreen->HandleMsg(hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LRESULT CALLBACK SplashScreen::WindowProcRedirect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        SplashScreen* pSplashScreen = reinterpret_cast<SplashScreen*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        return pSplashScreen->HandleMsg(hWnd, message, wParam, lParam);
    }

    LRESULT SplashScreen::HandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            m_hBitmap = (HBITMAP)LoadImage(m_hInstance, m_ImageName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            break;
        case WM_PAINT:
            PAINTSTRUCT     ps;
            HDC             hdc;
            BITMAP          bitmap;
            HDC             hdcMem;
            HGDIOBJ         oldBitmap;

            hdc = BeginPaint(hWnd, &ps);

            hdcMem = CreateCompatibleDC(hdc);
            oldBitmap = SelectObject(hdcMem, m_hBitmap);

            GetObject(m_hBitmap, sizeof(bitmap), &bitmap);
            StretchBlt(hdc, 0, 0, m_Width, m_Height, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);

            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            DeleteObject(m_hBitmap);
            //PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }
}