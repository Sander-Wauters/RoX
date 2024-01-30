#include <tuple>

#include "../Inc/RoX/Window.h"
#include "util/Logger.h"
#include "Exceptions/ThrowIfFailed.h"

Window::Window(Renderer& renderer, PCWSTR windowName, HINSTANCE hInstance,
        DWORD style, DWORD exStyle,
        int x, int y,
        int width, int height,
        HWND parent, HMENU menu
        ) : m_renderer(renderer), m_width(width), m_height(height) {
    if (!DirectX::XMVerifyCPUSupport())
        ThrowIfFailed(E_NOTIMPL);

    ThrowIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED));

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Window::WindowProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
    wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszClassName = windowName;
    wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
    if (!RegisterClassExW(&wcex))
        return;

    RECT wr = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
    AdjustWindowRect(&wr, style, menu == 0 ? FALSE : TRUE);

    m_hwnd = CreateWindowExW(
            exStyle, windowName, windowName, style,
            x, y,
            wr.right - wr.left,
            wr.bottom - wr.top,
            parent, menu,
            GetModuleHandle(NULL), this);

    m_width = wr.right - wr.left;
    m_height = wr.bottom - wr.top;

    if (!m_hwnd)
        return;

    Logger::Info("window initialized");
}

Window::~Window() {}

HWND Window::GetHwnd() const {
    return m_hwnd;
}

int Window::GetWidth() const {
    return m_width;
}

int Window::GetHeight() const {
    return m_height;
}

float Window::GetAspectRatio() const {
    return (float)m_width / m_height;
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false; // Set s_fullscreen to true if defaulting to fullscreen.
                                      //
    switch (msg) {
        case WM_PAINT:
            if (s_in_sizemove) {
                m_renderer.Update();
                m_renderer.Render();
            } else {
                PAINTSTRUCT ps;
                std::ignore = BeginPaint(m_hwnd, &ps);
                EndPaint(m_hwnd, &ps);
            }
            break;

        case WM_DISPLAYCHANGE:
            m_renderer.OnDisplayChange();
            break;

        case WM_MOVE:
            m_renderer.OnWindowMoved();
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED) {
                if (!s_minimized) {
                    s_minimized = true;
                    if (!s_in_suspend)
                        m_renderer.OnSuspending();
                    s_in_suspend = true;
                }
            } else if (s_minimized) {
                s_minimized = false;
                if (s_in_suspend)
                    m_renderer.OnResuming();
                s_in_suspend = false;
            } else if (!s_in_sizemove) {
                m_renderer.OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
            }
            break;

        case WM_ENTERSIZEMOVE:
            s_in_sizemove = true;
            break;

        case WM_EXITSIZEMOVE:
            s_in_sizemove = false;
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            m_renderer.OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
            break;

        case WM_GETMINMAXINFO:
            if (lParam) {
                MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
                info->ptMinTrackSize.x = 320;
                info->ptMinTrackSize.y = 200;
            }
            break;

        case WM_ACTIVATEAPP:
            if (wParam)
                m_renderer.OnActivated();
            else
                m_renderer.OnDeactivated();

            break;

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMQUERYSUSPEND:
                    if (!s_in_suspend)
                        m_renderer.OnSuspending();

                    s_in_suspend = true;
                    return TRUE;

                case PBT_APMRESUMESUSPEND:
                    if (!s_minimized) {
                        if (s_in_suspend)
                            m_renderer.OnResuming();
                        s_in_suspend = false;
                    }
                    return TRUE;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SYSKEYDOWN:
            if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
                // Implements the classic ALT+ENTER fullscreen toggle
                if (s_fullscreen) {
                    SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, 0);

                    ShowWindow(m_hwnd, SW_SHOWNORMAL);
                    SetWindowPos(m_hwnd, HWND_TOP, 0, 0, m_width, m_height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                } else {
                    SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_POPUP);
                    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                    SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                    ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
                }
                s_fullscreen = !s_fullscreen;
            }
            break;

        case WM_MENUCHAR:
            // A menu is active and the user presses a key that does not correspond
            // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
            return MAKELRESULT(0, MNC_CLOSE);

        default:
            return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }


    return TRUE;
}


