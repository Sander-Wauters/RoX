#include "RoX/Renderer.h"

#include "Exceptions/ThrowIfFailed.h"

Renderer::Renderer(Timer& timer) noexcept : 
m_timer(timer) {
    m_pDeviceResources = std::make_unique<DeviceResources>();
    m_pDeviceResources->RegisterDeviceNotify(this);
}

Renderer::~Renderer() {
    if (m_pDeviceResources)
        m_pDeviceResources->WaitForGpu();
}

void Renderer::Initialize(HWND window, int width, int height) {
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();
    m_pDeviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
}

void Renderer::Update() {

}

void Renderer::Render() {
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    // Prepare the command list to render a new frame.
    m_pDeviceResources->Prepare();
    Clear();

    ID3D12GraphicsCommandList* commandList = m_pDeviceResources->GetCommandList();

    // Add your rendering code here.


    // Show the new frame.
    m_pDeviceResources->Present();

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    // m_graphicsMemory->Commit(m_pDeviceResources->GetCommandQueue());
}

void Renderer::OnDeviceLost() {
    // Add Direct3D resource cleanup here.

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    // m_graphicsMemory.reset();
}

void Renderer::OnDeviceRestored() {
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void Renderer::OnActivated() {
    // Game is becoming active window.
}

void Renderer::OnDeactivated() {
    // Game is becoming background window.
}

void Renderer::OnSuspending() {
    // Game is being power-suspended (or minimized).
}

void Renderer::OnResuming() {
    m_timer.ResetElapsedTime();
    // Game is being power-resumed (or returning from minimize).
}

void Renderer::OnWindowMoved() {
    RECT const r = m_pDeviceResources->GetOutputSize();
    m_pDeviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Renderer::OnDisplayChange() {
    m_pDeviceResources->UpdateColorSpace();
}

void Renderer::OnWindowSizeChanged(int width, int height) {
    if (!m_pDeviceResources->WindowSizeChanged(width, height))
        return;
    CreateWindowSizeDependentResources();
    // Game window is being resized.
}

void Renderer::Clear() {
    ID3D12GraphicsCommandList* commandList = m_pDeviceResources->GetCommandList();

    // Clear the views.
    CD3DX12_CPU_DESCRIPTOR_HANDLE const rtvDescriptor = m_pDeviceResources->GetRenderTargetView();
    CD3DX12_CPU_DESCRIPTOR_HANDLE const dsvDescriptor = m_pDeviceResources->GetDepthStencilView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    commandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::LightSteelBlue, 0, nullptr);
    commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    D3D12_VIEWPORT const viewport = m_pDeviceResources->GetScreenViewport();
    D3D12_RECT const scissorRect = m_pDeviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

}

void Renderer::CreateDeviceDependentResources() {
    ID3D12Device* device = m_pDeviceResources->GetDevice();

    // Check Shader Model 6 support
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
        || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)) 
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
        throw std::runtime_error("Shader Model 6.0 is not supported!");
    }

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    // m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

    // Initialize device dependent objects here (independent of window size).

}

void Renderer::CreateWindowSizeDependentResources() {
    // Initialize window-size dependent objects hare.
    //
    // If using the DirectX Tool Kit for DX12, uncomment this line:
    // m_graphicsMemory.reset();
}
