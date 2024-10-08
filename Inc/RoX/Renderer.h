#pragma once

#include <functional>

#include "Scene.h"
#include "Window.h"

// Responsible for rendering the contents of a **Scene** to the display.
class Renderer : public IWindowObserver {
    public:
        Renderer(Window& window) noexcept;
        ~Renderer() noexcept;

        Renderer(Renderer&& other) noexcept;

        Renderer(Renderer const&) = delete;
        Renderer& operator= (Renderer const&) = delete;

    public:
        void Load(Scene& scene);

        void Update();
        void Render();
        void Render(const std::function<void()>& renderImGui);

        void ForceDeviceReset();

        void OnActivated() override;
        void OnDeactivated() override;
        void OnSuspending() override;
        void OnResuming() override;
        void OnWindowMoved() override;
        void OnDisplayChanged() override;
        void OnWindowSizeChanged(int width, int height) override;

    public:
        void SetMsaa(bool state) noexcept;
        bool IsMsaaEnabled() const noexcept;

    private:
        // Private implementation.
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
};
