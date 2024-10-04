#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <unordered_set>

#include "../../Lib/DirectXTK12/Inc/Keyboard.h"
#include "../../Lib/DirectXTK12/Inc/Mouse.h"

// Abstraction over window events.
// Used for when classes need to execute code when a certain window event is fired.
class IWindowObserver {
    public:   
        virtual void OnActivated() = 0;
        virtual void OnDeactivated() = 0;
        virtual void OnSuspending() = 0;
        virtual void OnResuming() = 0;
        virtual void OnWindowMoved() = 0;
        virtual void OnDisplayChanged() = 0;
        virtual void OnWindowSizeChanged(int width, int height) = 0;
};

// A window to be used be **Renderer**.
// Handles all window events, to execute code when a event fires use the **IWindowObserver** class.
class Window {
    public:
        Window(PCWSTR windowName, HINSTANCE hInstance,
                DWORD style, DWORD exStyle = 0,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int width = 1024, int height = 640,
                HWND parent = 0, HMENU menu = 0
              );
        ~Window();

    public:
        DirectX::Mouse& GetMouse() const noexcept;
        DirectX::Keyboard& GetKeyboard() const noexcept;

        HWND GetHwnd() const noexcept;
        int GetWidth() const noexcept;
        int GetHeight() const noexcept;
        float GetAspectRatio() const noexcept;

        void Attach(IWindowObserver* pIWindowObserver);
        void Detach(IWindowObserver* pIWindowObserver) noexcept;
        void DetachAll() noexcept;

    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
            Window *pThis = NULL;

            if (msg == WM_NCCREATE) {
                CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
                pThis = (Window*)pCreate->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

                pThis->m_hwnd = hwnd;
            } else
                pThis = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

            if (pThis)
                return pThis->HandleMessage(msg, wParam, lParam);
            else
                return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        void HandleActivated();
        void HandleDeactivated();
        void HandleSuspending();
        void HandleResuming();
        void HandleWindowMoved();
        void HandleDisplayChanged();
        void HandleWindowSizeChanged(int width, int height);

        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        std::unique_ptr<DirectX::Keyboard> m_pKeyboard;
        std::unique_ptr<DirectX::Mouse> m_pMouse;

        HWND m_hwnd = 0;

        int m_width = 0;
        int m_height = 0;

        std::unordered_set<IWindowObserver*> m_windowObservers;
};
