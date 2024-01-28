#pragma once
#include <windows.h>

class Renderer {
    public:
        Renderer();
        ~Renderer();

    public: // Initialization and management.
        void Initialize(HWND window, int width, int height);
        void Update();
        void Draw();

    public: // Messages
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowSizeChanged(int width, int height);
};
