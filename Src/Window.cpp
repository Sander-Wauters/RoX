#include "RoX/Window.h"

Window::Window(PCWSTR windowName, DWORD style, DWORD exStyle,
        int x, int y,
        int width, int height,
        HWND parent, HMENU menu
        ) : m_width(width), m_height(height) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = Window::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = windowName;
    RegisterClass(&wc);

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, style, menu == 0 ? FALSE : TRUE);

    m_hwnd = CreateWindowEx(
            exStyle,
            windowName,
            windowName,
            style,
            x, y,
            wr.right - wr.left,
            wr.bottom - wr.top,
            parent,
            menu,
            GetModuleHandle(NULL),
            this
            );
    
    Logger::Info("window initialized");
}

Window::~Window() {}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            m_width = LOWORD(lParam);
            m_height = HIWORD(lParam);
            m_isResized = true;
            m_isMinimized = wParam == SIZE_MINIMIZED;
            return 0;
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
            m_mouseButtonState.LBtnDown = (wParam & MK_LBUTTON) != 0;
            return 0;
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
            m_mouseButtonState.MBtnDown = (wParam & MK_MBUTTON) != 0;
            return 0;
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
            m_mouseButtonState.RBtnDown = (wParam & MK_RBUTTON) != 0;
            return 0;
        case WM_MOUSEMOVE:
            m_mousePosition.X = GET_X_LPARAM(lParam);
            m_mousePosition.Y = GET_Y_LPARAM(lParam);
            return 0;
        default:
            return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }

    return TRUE;
}


