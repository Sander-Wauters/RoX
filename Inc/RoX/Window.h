#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

#include "../../Lib/DirectXTK12/Inc/Keyboard.h"
#include "../../Lib/DirectXTK12/Inc/Mouse.h"

#include "../../Src/Util/pch.h"
#include "Renderer.h"

class Window {
    public:
        Window(Renderer& renderer, PCWSTR windowName, HINSTANCE hInstance,
                DWORD style, DWORD exStyle = 0,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int width = 1024, int height = 640,
                HWND parent = 0, HMENU menu = 0
              );
        ~Window();

    public: // Properties.
        DirectX::Mouse& GetMouse() const noexcept;
        DirectX::Keyboard& GetKeyboard() const noexcept;

        HWND GetHwnd() const noexcept;
        int GetWidth() const noexcept;
        int GetHeight() const noexcept;
        float GetAspectRatio() const noexcept;

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

        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        Renderer& m_renderer;

        std::unique_ptr<DirectX::Keyboard> m_pKeyboard = nullptr;
        std::unique_ptr<DirectX::Mouse> m_pMouse = nullptr;

        HWND m_hwnd = 0;

        int m_width = 0;
        int m_height = 0;
};
