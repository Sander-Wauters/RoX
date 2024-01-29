#include "RoX/Renderer.h"
#include "Exceptions/ThrowIfFailed.h"
#include "Util/Logger.h"

Renderer::Renderer(Timer& timer) noexcept : 
    m_timer(timer),
    m_window(nullptr),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0),
    m_backBufferIndex(0),
    m_rtvDescriptorSize(0),
    m_fenceValues{} {
    }

Renderer::~Renderer() {
    WaitForGpu();
}

void Renderer::Initialize(HWND window, int width, int height) {
    m_window = window;
    m_outputWidth = width;
    m_outputHeight = height;

    CreateDevice();
    CreateResources();
    Logger::Info("renderer initialized");
}

void Renderer::Update() {

}

void Renderer::Render() {
    // Do not render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;
    
    Clear();

    Present();
}

void Renderer::OnActivated() {

}

void Renderer::OnDeactivated() {

}

void Renderer::OnSuspending() {

}

void Renderer::OnResuming() {
    m_timer.ResetElapsedTime();
}

void Renderer::OnWindowSizeChanged(int width, int height) {
    if(!m_window)
        return;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);
    CreateResources();
}

void Renderer::CreateDevice() {    

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();
        }

    }
#endif

    DWORD dxgiFactoryFlags = 0;
    ThrowIfFailed(CreateDXGIFactory2(
                dxgiFactoryFlags, IID_PPV_ARGS(m_pDxgiFactory.ReleaseAndGetAddressOf())));

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    GetAdapter(adapter.GetAddressOf());

    // Create the DX12 API device object.
    ThrowIfFailed(D3D12CreateDevice(
                adapter.Get(),
                m_featureLevel,
                IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf())));

#ifndef NDEBUG
    // Configure debug device (if active).
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> d3dInfoQueue;
    if (SUCCEEDED(m_pDevice.As(&d3dInfoQueue)))
    {
#ifdef _DEBUG
        d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
        D3D12_MESSAGE_ID hide[] =
        {
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

    // Create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_pCommandQueue.ReleaseAndGetAddressOf())));

    // Create descriptor heaps for render target views and depth stencil views.
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
    rtvDescriptorHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
    dsvDescriptorHeapDesc.NumDescriptors = 1;
    dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(m_pRtvDescriptorHeap.ReleaseAndGetAddressOf())));
    ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(m_pDsvDescriptorHeap.ReleaseAndGetAddressOf())));

    m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create a command allocator for each back buffer that will be rendered to.
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
        ThrowIfFailed(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_pCommandAllocators[i].ReleaseAndGetAddressOf())));
    }

    // Create a command list for recording graphics commands.
    ThrowIfFailed(m_pDevice->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, 
                m_pCommandAllocators[0].Get(), nullptr, 
                IID_PPV_ARGS(m_pCommandList.ReleaseAndGetAddressOf())));
    ThrowIfFailed(m_pCommandList->Close());

    // Create a fence for tracking GPU execution progress.
    ThrowIfFailed(m_pDevice->CreateFence(
                m_fenceValues[m_backBufferIndex]++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf())));

    m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
    if (!m_fenceEvent.IsValid()) {
        throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "CreateEventEx");
    }

    // Check Shader Model 6 support
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
    if (FAILED(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
            || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0))
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
        throw std::runtime_error("Shader Model 6.0 is not supported!");
    }
}

void Renderer::CreateResources() {

    // Wait until all previous GPU work is complete.
    WaitForGpu();

    // Release resources that are tied to the swap chain and update fence values.
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
        m_pRenderTargets[i].Reset();
        m_fenceValues[i] = m_fenceValues[m_backBufferIndex];
    }

    constexpr DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    constexpr DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_pSwapChain) {
        HRESULT hr = m_pSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method
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
        swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

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

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        ThrowIfFailed(m_pDxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the back buffers for this window which will be the final render targets
    // and create render target views for each of them.
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
        ThrowIfFailed(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pRenderTargets[i].GetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", i);
        m_pRenderTargets[i]->SetName(name);

#ifdef __MINGW32__
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        std::ignore = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&cpuHandle);
#else
        auto cpuHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#endif

        const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(cpuHandle, static_cast<INT>(i), m_rtvDescriptorSize);
        m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvDescriptor);
    }

    // Reset the index to the current back buffer.
    m_backBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
    // on this surface.
    const CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            depthBufferFormat,
            backBufferWidth,
            backBufferHeight,
            1, // This depth stencil view has only one texture.
            1);  // Use a single mipmap level.
    depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    const CD3DX12_CLEAR_VALUE depthOptimizedClearValue(depthBufferFormat, 1.0f, 0u);

    ThrowIfFailed(m_pDevice->CreateCommittedResource(
                &depthHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &depthStencilDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &depthOptimizedClearValue,
                IID_PPV_ARGS(m_pDepthStencil.ReleaseAndGetAddressOf())));

    m_pDepthStencil->SetName(L"Depth stencil");

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthBufferFormat;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

#ifdef __MINGW32__
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    std::ignore = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&cpuHandle);
#else
    auto cpuHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#endif

    m_pDevice->CreateDepthStencilView(m_pDepthStencil.Get(), &dsvDesc, cpuHandle);

    Logger::Info("created resources");
}

void Renderer::Clear() {
    // Reset command list and allocator.
    ThrowIfFailed(m_pCommandAllocators[m_backBufferIndex]->Reset());
    ThrowIfFailed(m_pCommandList->Reset(m_pCommandAllocators[m_backBufferIndex].Get(), nullptr));

    // Transition the render target into the correct state to allow for drawing into it.
    const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pRenderTargets[m_backBufferIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_pCommandList->ResourceBarrier(1, &barrier);

    // Clear the views.
#ifdef __MINGW32__
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, cpuHandleDSV;
    std::ignore = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&cpuHandle);
    std::ignore = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&cpuHandleDSV);
#else
    auto cpuHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto cpuHandleDSV = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#endif

    const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(cpuHandle, static_cast<INT>(m_backBufferIndex), m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvDescriptor(cpuHandleDSV);
    m_pCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    m_pCommandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::LightSteelBlue, 0, nullptr);
    m_pCommandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    const D3D12_VIEWPORT viewport = { 
        0.0f, 0.0f, 
        static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight), 
        D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
    const D3D12_RECT scissorRect = { 
        0, 0, static_cast<LONG>(m_outputWidth), static_cast<LONG>(m_outputHeight) };
    m_pCommandList->RSSetViewports(1, &viewport);
    m_pCommandList->RSSetScissorRects(1, &scissorRect);
}

void Renderer::Present() {
    // Transition the render target to the state that allows it to be presented to the display.
    const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pRenderTargets[m_backBufferIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_pCommandList->ResourceBarrier(1, &barrier);

    // Send the command list off to the GPU for processing.
    ThrowIfFailed(m_pCommandList->Close());
    m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(m_pCommandList.GetAddressOf()));

    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_pSwapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        OnDeviceLost();
    else {
        ThrowIfFailed(hr);
        MoveToNextFrame();
    }
}

void Renderer::WaitForGpu() noexcept {
    if (m_pCommandQueue && m_pFence && m_fenceEvent.IsValid()) {
        // Schedule a Signal command in the GPU queue.
        UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
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

void Renderer::MoveToNextFrame() {
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

void Renderer::GetAdapter(IDXGIAdapter1** ppAdapter) {
    *ppAdapter = nullptr;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0; 
            DXGI_ERROR_NOT_FOUND != m_pDxgiFactory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf()); 
            ++adapterIndex) {
        DXGI_ADAPTER_DESC1 desc;
        ThrowIfFailed(adapter->GetDesc1(&desc));

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) // Don't select the Basic Render Driver adapter.
            continue;

        // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), m_featureLevel, __uuidof(ID3D12Device), nullptr)))
            break;
    }

#if !defined(NDEBUG)
    if (!adapter) {
        if (FAILED(m_pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
            throw std::runtime_error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
    }
#endif

    if (!adapter)
        throw std::runtime_error("No Direct3D 12 device found");

    *ppAdapter = adapter.Detach();
}

void Renderer::OnDeviceLost() {
    Logger::Info("device lost");

    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i) {
        m_pCommandAllocators[i].Reset();
        m_pRenderTargets[i].Reset();
    }

    m_pDepthStencil.Reset();
    m_pFence.Reset();
    m_pCommandList.Reset();
    m_pSwapChain.Reset();
    m_pRtvDescriptorHeap.Reset();
    m_pDsvDescriptorHeap.Reset();
    m_pCommandQueue.Reset();
    m_pDevice.Reset();
    m_pDxgiFactory.Reset();

    CreateDevice();
    CreateResources();
}

