#include "RoX/Window.h"

#include "ImGuiBackends/imgui_impl_win32.h"

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

Window::Window(PCWSTR windowName, HINSTANCE hInstance,
        DWORD style, DWORD exStyle,
        int x, int y,
        int width, int height,
        HWND parent, HMENU menu
        ) : m_width(width), m_height(height) 
{
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

    m_pKeyboard = std::make_unique<DirectX::Keyboard>();
    m_pMouse = std::make_unique<DirectX::Mouse>();
    m_pMouse->SetWindow(m_hwnd);
}

Window::~Window() {

}

DirectX::Mouse& Window::GetMouse() const noexcept {
    return *m_pMouse.get();
}

DirectX::Keyboard& Window::GetKeyboard() const noexcept {
    return *m_pKeyboard.get();
}

HWND Window::GetHwnd() const noexcept {
    return m_hwnd;
}

int Window::GetWidth() const noexcept {
    return m_width;
}

int Window::GetHeight() const noexcept {
    return m_height;
}

float Window::GetAspectRatio() const noexcept {
    return (float)m_width / m_height;
}

void Window::Attach(IWindowObserver* pIWindowObserver) {
    if (!pIWindowObserver)
        throw std::invalid_argument("IWindowObserver is nullptr.");
    m_windowObservers.insert(pIWindowObserver);
}

void Window::Detach(IWindowObserver* pIWindowObserver) noexcept {
    m_windowObservers.erase(pIWindowObserver);
}

void Window::HandleActivated() {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnActivated();
    }
}

void Window::HandleDeactivated() {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnDeactivated();
    }
}

void Window::HandleSuspending() {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnSuspending();
    }
}

void Window::HandleResuming() {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnResuming();
    }
}

void Window::HandleWindowMoved() {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnWindowMoved();
    }
}

void Window::HandleDisplayChanged() {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnDisplayChanged();
    }
}

void Window::HandleWindowSizeChanged(int width, int height) {
    for (IWindowObserver* pIWindowObserver : m_windowObservers) {
        if (pIWindowObserver)
            pIWindowObserver->OnWindowSizeChanged(width, height);
    }
}

LRESULT Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false; // Set s_fullscreen to true if defaulting to fullscreen.
                                      
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND mhwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(m_hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_PAINT:
            PAINTSTRUCT ps;
            std::ignore = BeginPaint(m_hwnd, &ps);
            EndPaint(m_hwnd, &ps);
            break;

        case WM_DISPLAYCHANGE:
            HandleDisplayChanged();
            break;

        case WM_MOVE:
            HandleWindowMoved();
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED) {
                if (!s_minimized) {
                    s_minimized = true;
                    if (!s_in_suspend)
                        HandleSuspending();
                    s_in_suspend = true;
                }
            } else if (s_minimized) {
                s_minimized = false;
                if (s_in_suspend)
                    HandleResuming();
                s_in_suspend = false;
            } else if (!s_in_sizemove) {
                HandleWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
            }
            break;

        case WM_ENTERSIZEMOVE:
            s_in_sizemove = true;
            break;

        case WM_EXITSIZEMOVE:
            s_in_sizemove = false;
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            HandleWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
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
                HandleActivated();
            else
                HandleDeactivated();

            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            DirectX::Mouse::ProcessMessage(msg, wParam, lParam);

            break;

        case WM_POWERBROADCAST:
            switch (wParam) {
                case PBT_APMQUERYSUSPEND:
                    if (!s_in_suspend)
                        HandleSuspending();

                    s_in_suspend = true;
                    return TRUE;

                case PBT_APMRESUMESUSPEND:
                    if (!s_minimized) {
                        if (s_in_suspend)
                            HandleResuming();
                        s_in_suspend = false;
                    }
                    return TRUE;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_MOUSEACTIVATE:
            // When you click activate the window, we want Mouse to ignore it.
            return MA_ACTIVATEANDEAT;

        case WM_ACTIVATE:
        case WM_INPUT:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_MOUSEHOVER:
            DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
            break;

        case WM_SYSKEYDOWN:
            DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);

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


