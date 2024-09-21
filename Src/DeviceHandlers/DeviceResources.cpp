#include "DeviceResources.h"

#include "../Util/dxtk12pch.h"

namespace {
    inline DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept {
        switch (fmt) {
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            default:                                
                return fmt;
        }
    }

    inline long ComputeIntersectionArea(
            long ax1, long ay1, long ax2, long ay2,
            long bx1, long by1, long bx2, long by2) noexcept {
        return std::max(0l, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0l, std::min(ay2, by2) - std::max(ay1, by1));
    }
}

DeviceResources::DeviceResources(
        DXGI_FORMAT backBufferFormat,
        DXGI_FORMAT depthBufferFormat,
        UINT backBufferCount,
        D3D_FEATURE_LEVEL minFeatureLevel,
        unsigned int flags) :
    m_backBufferIndex(0),
    m_fenceValues{},
    m_rtvDescriptorSize(0),
    m_screenViewport{},
    m_scissorRect{},
    m_backBufferFormat(backBufferFormat),
    m_depthBufferFormat(depthBufferFormat),
    m_backBufferCount(backBufferCount),
    m_d3dMinFeatureLevel(minFeatureLevel),
    m_window(nullptr),
    m_d3dFeatureLevel(D3D_FEATURE_LEVEL_11_0),
    m_dxgiFactoryFlags(0),
    m_outputSize{0, 0, 1, 1},
    m_colorSpace(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709),
    m_options(flags),
    m_deviceObservers({}) 
{
    if (backBufferCount < 2 || backBufferCount > MAX_BACK_BUFFER_COUNT)
        throw std::out_of_range("invalid backBufferCount");
    if (minFeatureLevel < D3D_FEATURE_LEVEL_11_0)
        throw std::out_of_range("minFeatureLevel too low");
}

DeviceResources::~DeviceResources() {
    // Ensure that the GPU is no longer referencing resources that are about to be destroyed.
    WaitForGpu();
}

void DeviceResources::CreateDeviceResources() {
#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> pDebugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.GetAddressOf()))))
            pDebugController->EnableDebugLayer();
        else
            OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_pDxgiFactory.ReleaseAndGetAddressOf())));

    // Determines whether tearing support is available for fullscreen borderless windows.
    if (m_options & ALLOW_TEARING) {
        BOOL allowTearing = FALSE;

        Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
        HRESULT hr = m_pDxgiFactory.As(&factory5);
        if (SUCCEEDED(hr))
            hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));

        if (FAILED(hr) || !allowTearing) {
            m_options &= ~ALLOW_TEARING;
#ifdef _DEBUG
            OutputDebugStringA("WARNING: Variable refresh rate displays not supported");
#endif
        }
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    GetAdapter(adapter.GetAddressOf());

    // Create the DX12 API device object.
    HRESULT hr = D3D12CreateDevice(
            adapter.Get(),
            m_d3dMinFeatureLevel,
            IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));
    ThrowIfFailed(hr);

    m_pDevice->SetName(L"DeviceResources");

#ifndef NDEBUG
    // Configure debug device (if active).
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
    if (SUCCEEDED(m_pDevice.As(&d3dInfoQueue))) {
#ifdef _DEBUG
        d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
        D3D12_MESSAGE_ID hide[] = {
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            // Workarounds for debug layer issues on hybrid-graphics systems
            D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
        };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
        filter.DenyList.pIDList = hide;
        d3dInfoQueue->AddStorageFilterEntries(&filter);
    }
#endif

    // Determine maximum supported feature level for this device
    static const D3D_FEATURE_LEVEL s_featureLevels[] = {
#if defined(NTDDI_WIN10_FE) || defined(USING_D3D12_AGILITY_SDK)
        D3D_FEATURE_LEVEL_12_2,
#endif
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
        static_cast<UINT>(std::size(s_featureLevels)), s_featureLevels, D3D_FEATURE_LEVEL_11_0
    };

    hr = m_pDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
    if (SUCCEEDED(hr))
        m_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;
    else
        m_d3dFeatureLevel = m_d3dMinFeatureLevel;

    // Create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCommandQueue.ReleaseAndGetAddressOf())));

    m_pCommandQueue->SetName(L"DeviceResources");

    // Create descriptor heaps for render target views and depth stencil views.
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
    rtvDescriptorHeapDesc.NumDescriptors = m_backBufferCount;
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_pRtvDescriptorHeap.ReleaseAndGetAddressOf())));

    m_pRtvDescriptorHeap->SetName(L"DeviceResources");

    m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN) {
        D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
        dsvDescriptorHeapDesc.NumDescriptors = 1;
        dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

        ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(m_pDsvDescriptorHeap.ReleaseAndGetAddressOf())));

        m_pDsvDescriptorHeap->SetName(L"DeviceResources");
    }

    // Create a command allocator for each back buffer that will be rendered to.
    for (UINT i = 0; i < m_backBufferCount; ++i) {
        ThrowIfFailed(m_pDevice->CreateCommandAllocator(
                    D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCommandAllocators[i].ReleaseAndGetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", i);
        m_pCommandAllocators[i]->SetName(name);
    }

    // Create a command list for recording graphics commands.
    ThrowIfFailed(m_pDevice->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocators[0].Get(), nullptr, IID_PPV_ARGS(m_pCommandList.ReleaseAndGetAddressOf())));
    ThrowIfFailed(m_pCommandList->Close());

    m_pCommandList->SetName(L"DeviceResources");

    // Create a fence for tracking GPU execution progress.
    ThrowIfFailed(m_pDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf())));
    m_fenceValues[m_backBufferIndex]++;

    m_pFence->SetName(L"DeviceResources");

    m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
    if (!m_fenceEvent.IsValid())
        throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "CreateEventEx");

    // Create MSAA resources.
    D3D12_DESCRIPTOR_HEAP_DESC msaaRtvDescriptorHeapDesc = {};
    msaaRtvDescriptorHeapDesc.NumDescriptors = 1;
    msaaRtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    D3D12_DESCRIPTOR_HEAP_DESC msaaDsvDescriptorHeapDesc = {};
    msaaDsvDescriptorHeapDesc.NumDescriptors = 1;
    msaaDsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    ThrowIfFailed(m_pDevice->CreateDescriptorHeap(
                &msaaRtvDescriptorHeapDesc,
                IID_PPV_ARGS(m_pMsaaRtvDescriptorHeap.ReleaseAndGetAddressOf())));
    ThrowIfFailed(m_pDevice->CreateDescriptorHeap(
                &msaaDsvDescriptorHeapDesc,
                IID_PPV_ARGS(m_pMsaaDsvDescriptorHeap.ReleaseAndGetAddressOf())));
}

void DeviceResources::CreateWindowSizeDependentResources() {
    if (!m_window)
        throw std::logic_error("Call SetWindow with a valid Win32 window handle");

    // Wait until all previous GPU work is complete.
    WaitForGpu();

    // Release resources that are tied to the swap chain and update fence values.
    for (UINT i = 0; i < m_backBufferCount; ++i) {
        m_pRenderTargets[i].Reset();
        m_fenceValues[i] = m_fenceValues[m_backBufferIndex];
    }

    // Determine the render target size in pixels.
    const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(m_outputSize.right - m_outputSize.left), 1u);
    const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(m_outputSize.bottom - m_outputSize.top), 1u);
    const DXGI_FORMAT backBufferFormat = NoSRGB(m_backBufferFormat);

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_pSwapChain) {
        // If the swap chain already exists, resize it.
        HRESULT hr = m_pSwapChain->ResizeBuffers(
                m_backBufferCount,
                backBufferWidth,
                backBufferHeight,
                backBufferFormat,
                (m_options & ALLOW_TEARING) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
#ifdef _DEBUG
            char buff[64] = {};
            sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n",
                    static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pDevice->GetDeviceRemovedReason() : hr));
            OutputDebugStringA(buff);
#endif
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            HandleDeviceLost();

            // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
            // and correctly set up the new device.
            return;
        } else
            ThrowIfFailed(hr);
    } else {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = m_backBufferCount;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        swapChainDesc.Flags = (m_options & ALLOW_TEARING) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a swap chain for the window.
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(m_pDxgiFactory->CreateSwapChainForHwnd(
                    m_pCommandQueue.Get(),
                    m_window,
                    &swapChainDesc,
                    &fsSwapChainDesc,
                    nullptr,
                    swapChain.GetAddressOf()));

        ThrowIfFailed(swapChain.As(&m_pSwapChain));

        // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        ThrowIfFailed(m_pDxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Handle color space settings for HDR
    UpdateColorSpace();

    // Obtain the back buffers for this window which will be the final render targets
    // and create render target views for each of them.
    for (UINT i = 0; i < m_backBufferCount; ++i) {
        ThrowIfFailed(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pRenderTargets[i].GetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", i);
        m_pRenderTargets[i]->SetName(name);

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = m_backBufferFormat;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
                cpuHandle,
                static_cast<INT>(i), m_rtvDescriptorSize);
        m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), &rtvDesc, rtvDescriptor);
    }

    // Reset the index to the current back buffer.
    m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN) {
        // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
        // on this surface.
        const CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
                m_depthBufferFormat,
                backBufferWidth,
                backBufferHeight,
                1, // This depth stencil view has only one texture.
                1  // Use a single mipmap level.
                );
        depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        const CD3DX12_CLEAR_VALUE depthOptimizedClearValue(m_depthBufferFormat, (m_options & REVERSE_DEPTH) ? 0.0f : 1.0f, 0u);

        ThrowIfFailed(m_pDevice->CreateCommittedResource(
                    &depthHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &depthStencilDesc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    &depthOptimizedClearValue,
                    IID_PPV_ARGS(m_pDepthStencil.ReleaseAndGetAddressOf())));

        m_pDepthStencil->SetName(L"Depth stencil");

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = m_depthBufferFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

        m_pDevice->CreateDepthStencilView(m_pDepthStencil.Get(), &dsvDesc, cpuHandle);
    }

    // Set the 3D rendering viewport and scissor rectangle to target the entire window.
    m_screenViewport.TopLeftX = m_screenViewport.TopLeftY = 0.f;
    m_screenViewport.Width = static_cast<float>(backBufferWidth);
    m_screenViewport.Height = static_cast<float>(backBufferHeight);
    m_screenViewport.MinDepth = D3D12_MIN_DEPTH;
    m_screenViewport.MaxDepth = D3D12_MAX_DEPTH;

    m_scissorRect.left = m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(backBufferWidth);
    m_scissorRect.bottom = static_cast<LONG>(backBufferHeight);

    // Create the MSAA depth/stencil buffer.
    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    CD3DX12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            MSAA_DEPTH_FORMAT,
            static_cast<UINT>(m_scissorRect.right),
            static_cast<UINT>(m_scissorRect.bottom),
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level
            MSAA_COUNT,
            MSAA_QUALITY);
    depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = MSAA_DEPTH_FORMAT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    ThrowIfFailed(m_pDevice->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &depthStencilDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &depthOptimizedClearValue,
                IID_PPV_ARGS(m_pMsaaDepthStencil.ReleaseAndGetAddressOf())));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = MSAA_DEPTH_FORMAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

    m_pDevice->CreateDepthStencilView(m_pMsaaDepthStencil.Get(), &dsvDesc,
            m_pMsaaDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    // Create MSAA render target.
    CD3DX12_RESOURCE_DESC msaaRTDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            m_backBufferFormat,
            static_cast<UINT>(m_scissorRect.right),
            static_cast<UINT>(m_scissorRect.bottom),
            1, // This render target view has only one texture.
            1, // Use a single mipmap level
            MSAA_COUNT,
            MSAA_QUALITY);
    msaaRTDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE msaaOptimizedClearValue = {};
    msaaOptimizedClearValue.Format = m_backBufferFormat;
    memcpy(msaaOptimizedClearValue.Color, DirectX::Colors::CornflowerBlue, sizeof(float) * 4);

    ThrowIfFailed(m_pDevice->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &msaaRTDesc,
                D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
                &msaaOptimizedClearValue,
                IID_PPV_ARGS(m_pMsaaRenderTarget.ReleaseAndGetAddressOf())));

    m_pDevice->CreateRenderTargetView(m_pMsaaRenderTarget.Get(), nullptr,
            m_pMsaaRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

bool DeviceResources::WindowSizeChanged(int width, int height) {
    if (!m_window)
        return false;

    RECT newRc;
    newRc.left = newRc.top = 0;
    newRc.right = static_cast<long>(width);
    newRc.bottom = static_cast<long>(height);
    if (newRc.right == m_outputSize.right && newRc.bottom == m_outputSize.bottom) {
        // Handle color space settings for HDR.
        UpdateColorSpace();
        return false;
    }

    m_outputSize = newRc;
    CreateWindowSizeDependentResources();
    return true;
}

void DeviceResources::HandleDeviceLost() {
    for (IDeviceObserver* pIDeviceObserver : m_deviceObservers) {
        if (pIDeviceObserver)
            pIDeviceObserver->OnDeviceLost();
    }

    for (UINT i = 0; i < m_backBufferCount; ++i) {
        m_pCommandAllocators[i].Reset();
        m_pRenderTargets[i].Reset();
    }
    m_pMsaaRenderTarget.Reset();

    m_pDepthStencil.Reset();
    m_pMsaaDepthStencil.Reset();
    m_pCommandQueue.Reset();
    m_pCommandList.Reset();
    m_pFence.Reset();
    m_pRtvDescriptorHeap.Reset();
    m_pDsvDescriptorHeap.Reset();
    m_pMsaaRtvDescriptorHeap.Reset();
    m_pMsaaDsvDescriptorHeap.Reset();
    m_pSwapChain.Reset();
    m_pDevice.Reset();
    m_pDxgiFactory.Reset();

#if defined(_DEBUG) && !defined(__MINGW32__)
    {
        Microsoft::WRL::ComPtr<IDXGIDebug1> pDxgiDebug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDxgiDebug))))
            pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    }
#endif

    CreateDeviceResources();
    CreateWindowSizeDependentResources();

    for (IDeviceObserver* pIDeviceObserver : m_deviceObservers) {
        if (pIDeviceObserver)
            pIDeviceObserver->OnDeviceRestored();
    }
}

void DeviceResources::Prepare(D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState) {
    // Reset command list and allocator.
    ThrowIfFailed(m_pCommandAllocators[m_backBufferIndex]->Reset());
    ThrowIfFailed(m_pCommandList->Reset(m_pCommandAllocators[m_backBufferIndex].Get(), nullptr));

    if (beforeState != afterState) {
        // Transition the render target into the correct state to allow for drawing into it.
        const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_pRenderTargets[m_backBufferIndex].Get(),
                beforeState, afterState);
        m_pCommandList->ResourceBarrier(1, &barrier);
    }
}

void DeviceResources::Present(D3D12_RESOURCE_STATES beforeState) {
    if (beforeState != D3D12_RESOURCE_STATE_PRESENT) {
        // Transition the render target to the state that allows it to be presented to the display.
        const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_pRenderTargets[m_backBufferIndex].Get(),
                beforeState, D3D12_RESOURCE_STATE_PRESENT);
        m_pCommandList->ResourceBarrier(1, &barrier);
    }

    // Send the command list off to the GPU for processing.
    ThrowIfFailed(m_pCommandList->Close());
    m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));

    HRESULT hr;
    if (m_options & ALLOW_TEARING) {
        // Recommended to always use tearing if supported when using a sync interval of 0.
        // Note this will fail if in true 'fullscreen' mode.
        hr = m_pSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
    } else {
        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        hr = m_pSwapChain->Present(1, 0);
    }

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
#ifdef _DEBUG
        char buff[64] = {};
        sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
                static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_pDevice->GetDeviceRemovedReason() : hr));
        OutputDebugStringA(buff);
#endif
        HandleDeviceLost();
    } else {
        ThrowIfFailed(hr);

        MoveToNextFrame();

        if (!m_pDxgiFactory->IsCurrent())
            UpdateColorSpace();
    }
}

void DeviceResources::WaitForGpu() noexcept {
    if (m_pCommandQueue && m_pFence && m_fenceEvent.IsValid()) {
        // Schedule a Signal command in the GPU queue.
        const UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
        if (SUCCEEDED(m_pCommandQueue->Signal(m_pFence.Get(), fenceValue))) {
            // Wait until the Signal has been processed.
            if (SUCCEEDED(m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get()))) {
                std::ignore = WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

                // Increment the fence value for the current frame.
                m_fenceValues[m_backBufferIndex]++;
            }
        }
    }
}

void DeviceResources::UpdateColorSpace() {
    if (!m_pDxgiFactory)
        return;

    // Output information is cached on the DXGI Factory. If it is stale we need to create a new factory.
    if (!m_pDxgiFactory->IsCurrent())
        ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(m_pDxgiFactory.ReleaseAndGetAddressOf())));

    DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

    bool isDisplayHDR10 = false;

    if (m_pSwapChain) {
        // To detect HDR support, we will need to check the color space in the primary
        // DXGI output associated with the app at this point in time
        // (using window/display intersection).

        // Get the retangle bounds of the app window.
        RECT windowBounds;
        if (!GetWindowRect(m_window, &windowBounds))
            throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetWindowRect");

        const long ax1 = windowBounds.left;
        const long ay1 = windowBounds.top;
        const long ax2 = windowBounds.right;
        const long ay2 = windowBounds.bottom;

        Microsoft::WRL::ComPtr<IDXGIOutput> bestOutput;
        long bestIntersectArea = -1;

        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        for (UINT adapterIndex = 0;
                SUCCEEDED(m_pDxgiFactory->EnumAdapters(adapterIndex, adapter.ReleaseAndGetAddressOf()));
                ++adapterIndex)
        {
            Microsoft::WRL::ComPtr<IDXGIOutput> output;
            for (UINT outputIndex = 0;
                    SUCCEEDED(adapter->EnumOutputs(outputIndex, output.ReleaseAndGetAddressOf()));
                    ++outputIndex)
            {
                // Get the rectangle bounds of current output.
                DXGI_OUTPUT_DESC desc;
                ThrowIfFailed(output->GetDesc(&desc));
                const RECT& r = desc.DesktopCoordinates;

                // Compute the intersection
                const long intersectArea = ComputeIntersectionArea(ax1, ay1, ax2, ay2, r.left, r.top, r.right, r.bottom);
                if (intersectArea > bestIntersectArea) {
                    bestOutput.Swap(output);
                    bestIntersectArea = intersectArea;
                }
            }
        }

        if (bestOutput) {
            Microsoft::WRL::ComPtr<IDXGIOutput6> output6;
            if (SUCCEEDED(bestOutput.As(&output6))) {
                DXGI_OUTPUT_DESC1 desc;
                ThrowIfFailed(output6->GetDesc1(&desc));

                // Display output is HDR10.
                if (desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
                    isDisplayHDR10 = true;
            }
        }
    }

    if ((m_options & ENABLE_HDR) && isDisplayHDR10) {
        switch (m_backBufferFormat) {
            case DXGI_FORMAT_R10G10B10A2_UNORM:
                // The application creates the HDR10 signal.
                colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
                break;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                // The system creates the HDR10 signal; application uses linear values.
                colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
                break;

            default:
                break;
        }
    }

    m_colorSpace = colorSpace;

    UINT colorSpaceSupport = 0;
    if (m_pSwapChain
            && SUCCEEDED(m_pSwapChain->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport))
            && (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
        ThrowIfFailed(m_pSwapChain->SetColorSpace1(colorSpace));

}

void DeviceResources::Attach(IDeviceObserver* pIDeviceObserver) {
    if (!pIDeviceObserver)
        throw std::invalid_argument("IDeviceObserver is nullptr.");
    m_deviceObservers.insert(pIDeviceObserver);
}

void DeviceResources::Detach(IDeviceObserver* pIDeviceObserver) noexcept {
    m_deviceObservers.erase(pIDeviceObserver);
}

void DeviceResources::SetWindow(HWND window, int width, int height) noexcept {
    m_window = window;

    m_outputSize.left = m_outputSize.top = 0;
    m_outputSize.right = static_cast<long>(width);
    m_outputSize.bottom = static_cast<long>(height);
}

RECT DeviceResources::GetOutputSize() const noexcept {
    return m_outputSize;
}

ID3D12Device* DeviceResources::GetDevice() const noexcept {
    return m_pDevice.Get();
}

IDXGISwapChain3* DeviceResources::GetSwapChain() const noexcept {
    return m_pSwapChain.Get();
}

IDXGIFactory4* DeviceResources::GetDxgiFactory() const noexcept {
    return m_pDxgiFactory.Get();
}

HWND DeviceResources::GetWindow() const noexcept {
    return m_window;
}

D3D_FEATURE_LEVEL DeviceResources::GetDeviceFeatureLevel() const noexcept {
    return m_d3dFeatureLevel;
}

ID3D12Resource* DeviceResources::GetRenderTarget() const noexcept {
    return m_pRenderTargets[m_backBufferIndex].Get();
}

ID3D12Resource* DeviceResources::GetDepthStencil() const noexcept {
    return m_pDepthStencil.Get();
}

ID3D12Resource* DeviceResources::GetMsaaRenderTarget() const noexcept {
    return m_pMsaaRenderTarget.Get();
}

ID3D12Resource* DeviceResources::GetMsaaDepthStencil() const noexcept {
    return m_pMsaaDepthStencil.Get();
}

ID3D12CommandQueue* DeviceResources::GetCommandQueue() const noexcept {
    return m_pCommandQueue.Get();
}

ID3D12CommandAllocator* DeviceResources::GetCommandAllocator() const noexcept {
    return m_pCommandAllocators[m_backBufferIndex].Get();
}

ID3D12GraphicsCommandList* DeviceResources::GetCommandList() const noexcept {
    return m_pCommandList.Get();
}

DXGI_FORMAT DeviceResources::GetBackBufferFormat() const noexcept {
    return m_backBufferFormat;
}

DXGI_FORMAT DeviceResources::GetDepthBufferFormat() const noexcept {
    return m_depthBufferFormat;
}

D3D12_VIEWPORT DeviceResources::GetScreenViewport() const noexcept {
    return m_screenViewport;
}

D3D12_RECT DeviceResources::GetScissorRect() const noexcept {
    return m_scissorRect;
}

UINT DeviceResources::GetCurrentFrameIndex() const noexcept {
    return m_backBufferIndex;
}

UINT DeviceResources::GetBackBufferCount() const noexcept {
    return m_backBufferCount;
}

DXGI_COLOR_SPACE_TYPE DeviceResources::GetColorSpace() const noexcept {
    return m_colorSpace;
}

unsigned int DeviceResources::GetDeviceOptions() const noexcept {
    return m_options;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetRenderTargetView() const noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandle, static_cast<INT>(m_backBufferIndex), m_rtvDescriptorSize);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetDepthStencilView() const noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandle);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetMsaaRenderTargetView() const noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pMsaaRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandle);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetMsaaDepthStencilView() const noexcept {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pMsaaDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandle);
}

void DeviceResources::MoveToNextFrame() {
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
    ThrowIfFailed(m_pCommandQueue->Signal(m_pFence.Get(), currentFenceValue));

    // Update the back buffer index.
    m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_pFence->GetCompletedValue() < m_fenceValues[m_backBufferIndex]) {
        ThrowIfFailed(m_pFence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
        std::ignore = WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
}

void DeviceResources::GetAdapter(IDXGIAdapter1** ppAdapter) {
    *ppAdapter = nullptr;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
    HRESULT hr = m_pDxgiFactory.As(&factory6);
    if (SUCCEEDED(hr)) {
        for (UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                        adapterIndex,
                        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                        IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));
                ++adapterIndex) 
        {
            DXGI_ADAPTER_DESC1 desc;
            ThrowIfFailed(adapter->GetDesc1(&desc));

            // Don't select the Basic Render Driver adapter.
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) 
                continue;

            // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_d3dMinFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
#ifdef _DEBUG
                wchar_t buff[256] = {};
                swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
                OutputDebugStringW(buff);
#endif
                break;
            }
        }
    }

    if (!adapter) {
        for (UINT adapterIndex = 0;
                SUCCEEDED(m_pDxgiFactory->EnumAdapters1(
                        adapterIndex,
                        adapter.ReleaseAndGetAddressOf()));
                ++adapterIndex) 
        {
            DXGI_ADAPTER_DESC1 desc;
            ThrowIfFailed(adapter->GetDesc1(&desc));

            // Don't select the Basic Render Driver adapter.
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_d3dMinFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
#ifdef _DEBUG
                wchar_t buff[256] = {};
                swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
                OutputDebugStringW(buff);
#endif
                break;
            }
        }
    }

#if !defined(NDEBUG)
    if (!adapter) {
        // Try WARP12 instead
        if (FAILED(m_pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
            throw std::runtime_error("WARP12 not available. Enable the 'Graphics Tools' optional feature");

        OutputDebugStringA("Direct3D Adapter - WARP12\n");
    }
#endif

    if (!adapter)
        throw std::runtime_error("No Direct3D 12 device found");

    *ppAdapter = adapter.Detach();
}
