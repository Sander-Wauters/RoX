#pragma once

#include "../Src/Util/pch.h"
#include "Timer.h"

class Renderer {
    public:
        Renderer(Timer& timer) noexcept;
        ~Renderer();

        Renderer(Renderer&&) = default;
        Renderer& operator= (Renderer&&) = default;

        Renderer(Renderer const&) = delete;
        Renderer& operator= (Renderer const&) = delete;

    public: // Initialization and management.
        void Initialize(HWND window, int width, int height);

        void Update();
        void Render();

    public: // Messages.
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowSizeChanged(int width, int height);

    public: // Properties.

    private:
        void CreateDevice();
        void CreateCommandObjects();
        void CreateResources();

        void Clear();
        void Present();

        void WaitForGpu() noexcept;
        void MoveToNextFrame();
        void GetAdapter(IDXGIAdapter1** ppAdapter);

        void OnDeviceLost();

    private: // Application state.
        Timer& m_timer;
        HWND m_window;
        int m_outputWidth;
        int m_outputHeight;

    private: // Direct3D Objects.    
        D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;
        static constexpr UINT SWAP_CHAIN_BUFFER_COUNT = 2;

        UINT  m_backBufferIndex = 0;
        UINT  m_rtvDescriptorSize = 0;

        Microsoft::WRL::ComPtr<ID3D12Device> m_pDevice = nullptr;
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDxgiFactory = nullptr;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue = nullptr;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pRtvDescriptorHeap = nullptr;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDsvDescriptorHeap = nullptr;

        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[SWAP_CHAIN_BUFFER_COUNT];
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr;

        Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence = nullptr;
        UINT64 m_fenceValues[SWAP_CHAIN_BUFFER_COUNT];
        Microsoft::WRL::Wrappers::Event m_fenceEvent = {};

    private: // Rendering resources
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargets[SWAP_CHAIN_BUFFER_COUNT];
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthStencil = nullptr;
};
