#pragma once

#include "../Src/Util/pch.h"
#include "../Src/IDeviceNotify.h"
#include "../Src/DeviceResources.h"
#include "Timer.h"

class Renderer : public IDeviceNotify {
    public:
        Renderer(Timer& timer) noexcept;
        ~Renderer();

        Renderer(Renderer&&) = default;

        Renderer(Renderer const&) = delete;
        Renderer& operator= (Renderer const&) = delete;

    public: // Initialization and management.
        void Initialize(HWND window, int width, int height);

        void Update();
        void Render();

    public: // Messages.
        void OnDeviceLost() override;
        void OnDeviceRestored() override;
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowMoved();
        void OnDisplayChange();
        void OnWindowSizeChanged(int width, int height);

    private:
        void Clear();

        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();

    private: // Application state.
        Timer& m_timer;
        std::unique_ptr<DeviceResources> m_pDeviceResources = nullptr;
};
