#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <utility>

#include "Renderer.h"

class Window {
    public:
        struct MousePosition {
            int X = 0;
            int Y = 0;
        };

        struct MouseButtonState {
            bool LBtnDown = false;
            bool MBtnDown = false;
            bool RBtnDown = false;
        };

    public:
        Window(PCWSTR windowName, DWORD style, DWORD exStyle = 0,
                int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int width = 1024, int height = 640,
                HWND parent = 0, HMENU menu = 0
              );
        ~Window();

        HWND GetHWND() const;
        UINT GetWidth() const;
        UINT GetHeight() const;
        float GetAspectRatio() const;

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
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }

        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        HWND m_hwnd = 0;

        int m_width = 0;
        int m_height = 0;

    private:
        Renderer& m_renderer;
};
