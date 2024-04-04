#pragma once

#include "RoX/IWindowObserver.h"

#include "Timer.h"
#include "Scene.h"

class Renderer : public IWindowObserver {
    public:
        Renderer(Timer& timer) noexcept;
        ~Renderer() noexcept;

        Renderer(Renderer&& moveFrom) noexcept;

        Renderer(Renderer const&) = delete;
        Renderer& operator= (Renderer const&) = delete;

    public:
        void Initialize(HWND window, int width, int height);
        void Load(Scene& scene);

        void Update();
        void Render();

    public:
        void OnActivated() override;
        void OnDeactivated() override;
        void OnSuspending() override;
        void OnResuming() override;
        void OnWindowMoved() override;
        void OnDisplayChange() override;
        void OnWindowSizeChanged(int width, int height) override;

    public:
        void SetMsaa(bool state) noexcept;
        bool IsMsaaEnabled() const noexcept;

    private:
        Timer& m_timer;

        // Private implementation.
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
};
