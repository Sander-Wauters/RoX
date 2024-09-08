#pragma once

#include "../Util/pch.h"
#include "IDeviceObserver.h"

class DeviceResources {
    public:
        DeviceResources(
                DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
                DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
                UINT backBufferCount = 2,
                D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0,
                unsigned int flags = 0);
        ~DeviceResources();

        DeviceResources(DeviceResources&&) = default;
        DeviceResources& operator= (DeviceResources&&) = default;

        DeviceResources(DeviceResources const&) = delete;
        DeviceResources& operator= (DeviceResources const&) = delete;

    public:
        void CreateDeviceResources();
        void CreateWindowSizeDependentResources();
        bool WindowSizeChanged(int width, int height);
        void HandleDeviceLost();
        void Prepare(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT,
                     D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_RENDER_TARGET);
        void Present(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);
        void WaitForGpu() noexcept;
        void UpdateColorSpace();

    public:
        void RegisterDeviceObserver(IDeviceObserver* pDeviceObserver) noexcept;
        void DeRegisterDeviceObserver(IDeviceObserver* pDeviceObserver) noexcept;
        void SetWindow(HWND window, int width, int height) noexcept;

        RECT GetOutputSize() const noexcept;
        ID3D12Device* GetDevice() const noexcept; 
        IDXGISwapChain3* GetSwapChain() const noexcept;         
        IDXGIFactory4* GetDxgiFactory() const noexcept;
        HWND GetWindow() const noexcept;
        D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const noexcept;
        ID3D12Resource* GetRenderTarget() const noexcept;
        ID3D12Resource* GetDepthStencil() const noexcept;
        ID3D12Resource* GetMsaaRenderTarget() const noexcept;
        ID3D12Resource* GetMsaaDepthStencil() const noexcept;
        ID3D12CommandQueue* GetCommandQueue() const noexcept;     
        ID3D12CommandAllocator* GetCommandAllocator() const noexcept;
        ID3D12GraphicsCommandList* GetCommandList() const noexcept;
        DXGI_FORMAT GetBackBufferFormat() const noexcept;
        DXGI_FORMAT GetDepthBufferFormat() const noexcept;
        D3D12_VIEWPORT GetScreenViewport() const noexcept;
        D3D12_RECT GetScissorRect() const noexcept;
        UINT GetCurrentFrameIndex() const noexcept;
        UINT GetBackBufferCount() const noexcept;
        DXGI_COLOR_SPACE_TYPE GetColorSpace() const noexcept;
        unsigned int GetDeviceOptions() const noexcept;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const noexcept;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const noexcept;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetMsaaRenderTargetView() const noexcept;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetMsaaDepthStencilView() const noexcept;

    private:
        void MoveToNextFrame();
        void GetAdapter(IDXGIAdapter1** ppAdapter);

    public:
        static constexpr unsigned int ALLOW_TEARING = 0x1;
        static constexpr unsigned int ENABLE_HDR = 0x2;
        static constexpr unsigned int REVERSE_DEPTH = 0x4;

        static constexpr UINT MSAA_COUNT = 4;
        static constexpr UINT MSAA_QUALITY = 0;
        static constexpr DXGI_FORMAT MSAA_DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;

    private:
        static constexpr size_t MAX_BACK_BUFFER_COUNT = 3;

        UINT m_backBufferIndex;

        // Direct3D objects.
        Microsoft::WRL::ComPtr<ID3D12Device> m_pDevice;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[MAX_BACK_BUFFER_COUNT];

        // Swap chain objects.
        Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDxgiFactory;
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pSwapChain;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargets[MAX_BACK_BUFFER_COUNT];
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthStencil;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pMsaaRenderTarget; 
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pMsaaDepthStencil;

        // Presentation fence objects.
        Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
        UINT64 m_fenceValues[MAX_BACK_BUFFER_COUNT];
        Microsoft::WRL::Wrappers::Event m_fenceEvent;

        // Direct3D rendering objects.
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pRtvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDsvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pMsaaRtvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pMsaaDsvDescriptorHeap;
        UINT m_rtvDescriptorSize;
        D3D12_VIEWPORT m_screenViewport;
        D3D12_RECT m_scissorRect;

        // Direct3D properties.
        DXGI_FORMAT m_backBufferFormat;
        DXGI_FORMAT m_depthBufferFormat;
        UINT m_backBufferCount;
        D3D_FEATURE_LEVEL m_d3dMinFeatureLevel;

        // Cached device properties.
        HWND m_window;
        D3D_FEATURE_LEVEL m_d3dFeatureLevel;
        DWORD m_dxgiFactoryFlags;
        RECT m_outputSize;

        // HDR Support
        DXGI_COLOR_SPACE_TYPE m_colorSpace;

        // DeviceResources options (see flags above)
        unsigned int m_options;

        std::unordered_set<IDeviceObserver*> m_deviceObservers;
};
