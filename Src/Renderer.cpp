#include "RoX/Renderer.h"

#include <CommonStates.h>
#include <ResourceUploadBatch.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <DirectXHelpers.h>
#include <RenderTargetState.h>

#include "Exceptions/ThrowIfFailed.h"
#include "Util/Logger.h"

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

    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    ID3D12DescriptorHeap* heaps[] = { m_pResourceDescriptors->Heap(), m_pStates->Heap() };
    pCommandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);

    m_pSpriteBatch->Begin(pCommandList, DirectX::SpriteSortMode_FrontToBack);

    // TEMP: font test.
    //const wchar_t* output = L"Hello World";
    /*
    std::string output = 
        "FPS: " + std::to_string(m_timer.GetFramesPerSecond()) + 
        "\nMS:  " + std::to_string(m_timer.GetElapsedSeconds() / 1000.0f);

    DirectX::SimpleMath::Vector2 origin = m_pFont->MeasureString(output.c_str());
    origin /= 2.0f;

    m_pFont->DrawString(m_pSpriteBatch.get(), output.c_str(),
            m_fontPos, DirectX::Colors::Black, 0.0f, origin, 2, DirectX::SpriteEffects_None, 0.3f);
    */

    RenderSprites();
    RenderText();


    m_pSpriteBatch->End();

    // Show the new frame.
    m_pDeviceResources->Present();
    m_pGraphicsMemory->Commit(m_pDeviceResources->GetCommandQueue());
}

void Renderer::AddSprite(Sprite* pSprite) {
    m_pSprite[pSprite] = m_nextSpriteDescriptorHeapIndex++;
}

void Renderer::AddText(Text* pText) {
    m_pText[pText] = m_nextSpriteDescriptorHeapIndex++;
}

void Renderer::OnDeviceLost() {
    // Add Direct3D resource cleanup here.
    m_pDeviceResources->HandleDeviceLost();
    m_pGraphicsMemory.reset();

    m_pResourceDescriptors.reset();
    m_pStates.reset();
    m_pSpriteBatch.reset();

    for (std::pair<const UINT, std::unique_ptr<SpriteData>>& pSprite : m_pSpriteData) {
        pSprite.second->pTexture.Reset();
    }
    for (std::pair<const UINT, std::unique_ptr<DirectX::SpriteFont>>& pFont : m_pTextData) {
        pFont.second.reset();
    }

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
    // Game is being power-resumed (or returning from minimize).
    m_timer.ResetElapsedTime();
}

void Renderer::OnWindowMoved() {
    RECT const r = m_pDeviceResources->GetOutputSize();
    m_pDeviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Renderer::OnDisplayChange() {
    m_pDeviceResources->UpdateColorSpace();
}

void Renderer::OnWindowSizeChanged(int width, int height) {
    // Game window is being resized.
    if (!m_pDeviceResources->WindowSizeChanged(width, height))
        return;
    CreateWindowSizeDependentResources();
}

void Renderer::Clear() {
    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    // Clear the views.
    CD3DX12_CPU_DESCRIPTOR_HANDLE const rtvDescriptor = m_pDeviceResources->GetRenderTargetView();
    CD3DX12_CPU_DESCRIPTOR_HANDLE const dsvDescriptor = m_pDeviceResources->GetDepthStencilView();

    pCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    pCommandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::LightSteelBlue, 0, nullptr);
    pCommandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    D3D12_VIEWPORT const viewport = m_pDeviceResources->GetScreenViewport();
    D3D12_RECT const scissorRect = m_pDeviceResources->GetScissorRect();
    pCommandList->RSSetViewports(1, &viewport);
    pCommandList->RSSetScissorRects(1, &scissorRect);
}

void Renderer::RenderSprites() {
    for (std::pair<Sprite* const, UINT>& pSprite : m_pSprite) {
        SpriteData* pSpriteData = m_pSpriteData.at(pSprite.second).get();

        m_pSpriteBatch->Draw(
                m_pResourceDescriptors->GetGpuHandle(pSprite.second),
                DirectX::GetTextureSize(pSpriteData->pTexture.Get()),
                pSpriteData->StretchRect,
                &pSpriteData->TileRect,
                pSpriteData->Tint,
                pSprite.first->Angle,
                pSpriteData->Origin,
                DirectX::SpriteEffects_None,
                pSprite.first->Layer);
    }
}

void Renderer::RenderText() {
    for (std::pair<Text* const, UINT>& pText : m_pText) {
        DirectX::SpriteFont* pFont = m_pTextData.at(pText.second).get(); 

        DirectX::SimpleMath::Vector2 origin = pFont->MeasureString(pText.first->Content.c_str());
        origin /= 2.0f;

        pFont->DrawString(
                m_pSpriteBatch.get(), 
                pText.first->Content.c_str(),
                m_fontPos, 
                DirectX::Colors::Black, 
                pText.first->Angle, 
                origin, 
                pText.first->Scale, 
                DirectX::SpriteEffects_None, 
                pText.first->Layer);
    } 
}

void Renderer::CreateDeviceDependentResources() {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    // Check Shader Model 6 support
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
    if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
            || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0)) 
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
        throw std::runtime_error("Shader Model 6.0 is not supported!");
    }

    m_pGraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(pDevice);

    m_pResourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(
            pDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            m_pSprite.size() + m_pText.size());
    m_pStates = std::make_unique<DirectX::CommonStates>(pDevice);

    BuildSpriteDataResources();
    BuildTextDataResources();

    // TEMP: font test.
    /*
    DirectX::ResourceUploadBatch resourceUpload(pDevice);

    resourceUpload.Begin();

    m_pFont = std::make_unique<DirectX::SpriteFont>(pDevice, resourceUpload,
            L"assets/courierNew12.spritefont",
            m_pResourceDescriptors->GetCpuHandle(m_nextSpriteDescriptorHeapIndex),
            m_pResourceDescriptors->GetGpuHandle(m_nextSpriteDescriptorHeapIndex));

    std::future<void> uploadResourcesFinished = resourceUpload.End(
            m_pDeviceResources->GetCommandQueue());

    DirectX::RenderTargetState rtState(m_pDeviceResources->GetBackBufferFormat(),
            m_pDeviceResources->GetDepthBufferFormat());

    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUpload, pd);

    uploadResourcesFinished.wait();
    */
}

void Renderer::BuildSpriteDataResources() {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    DirectX::RenderTargetState rtState(
            m_pDeviceResources->GetBackBufferFormat(),
            m_pDeviceResources->GetDepthBufferFormat());

    for (std::pair<Sprite* const, UINT>& pSprite : m_pSprite) {
        DirectX::ResourceUploadBatch resourceUpload(pDevice);
        resourceUpload.Begin();

        D3D12_GPU_DESCRIPTOR_HANDLE sampler = m_pStates->LinearWrap();
        DirectX::SpriteBatchPipelineStateDescription pd(rtState, nullptr, nullptr, nullptr, &sampler);
        m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUpload, pd);

        std::unique_ptr<SpriteData> pSpriteData = std::make_unique<SpriteData>();

        if (pSprite.first->FilePath.size() < 5)
            throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "BuildSpriteDataResources");

        std::wstring fileExtension = pSprite.first->FilePath.substr(pSprite.first->FilePath.size() - 4);
        if (fileExtension == L".png") {
            ThrowIfFailed(DirectX::CreateWICTextureFromFile(
                        pDevice, resourceUpload, pSprite.first->FilePath.c_str(), pSpriteData->pTexture.ReleaseAndGetAddressOf()));  
        } else if (fileExtension == L".dds") {
            ThrowIfFailed(DirectX::CreateDDSTextureFromFile(
                        pDevice, resourceUpload, pSprite.first->FilePath.c_str(), pSpriteData->pTexture.ReleaseAndGetAddressOf()));
        } else
            throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "BuildSpriteDataResources");

        DirectX::CreateShaderResourceView(pDevice, pSpriteData->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(pSprite.second));

        DirectX::RenderTargetState rtState(m_pDeviceResources->GetBackBufferFormat(), m_pDeviceResources->GetDepthBufferFormat());

        DirectX::XMUINT2 textureSize = DirectX::GetTextureSize(pSpriteData->pTexture.Get());

        // TODO: make the user set the origin.
        pSpriteData->Origin.x = float(textureSize.x / 2.0f);
        pSpriteData->Origin.y = float(textureSize.y / 2.0f);

        // TODO: figure out tiling.
        pSpriteData->TileRect.left = textureSize.x;
        pSpriteData->TileRect.right = textureSize.x * 2;
        pSpriteData->TileRect.top = textureSize.y;
        pSpriteData->TileRect.bottom = textureSize.y * 2;

        m_pSpriteData[pSprite.second] = std::move(pSpriteData);

        std::future<void> uploadResourceFinished = resourceUpload.End(m_pDeviceResources->GetCommandQueue());
        uploadResourceFinished.wait();
    }
}

void Renderer::BuildTextDataResources() {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    for (std::pair<Text* const, UINT> pText : m_pText) {
        DirectX::ResourceUploadBatch resourceUpload(pDevice);
        resourceUpload.Begin();

        std::unique_ptr<DirectX::SpriteFont> pFont = std::make_unique<DirectX::SpriteFont>(pDevice, resourceUpload,
                pText.first->FilePath.c_str(),
                m_pResourceDescriptors->GetCpuHandle(pText.second),
                m_pResourceDescriptors->GetGpuHandle(pText.second));

        std::future<void> uploadResourcesFinished = resourceUpload.End(
                m_pDeviceResources->GetCommandQueue());

        DirectX::RenderTargetState rtState(m_pDeviceResources->GetBackBufferFormat(),
                m_pDeviceResources->GetDepthBufferFormat());

        DirectX::SpriteBatchPipelineStateDescription pd(rtState);
        m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUpload, pd);

        m_pTextData[pText.second] = std::move(pFont);

        uploadResourcesFinished.wait();
    }
}

void Renderer::CreateWindowSizeDependentResources() {
    BuildSpriteDataSize();

    // TEMP: font test.
    auto viewport = m_pDeviceResources->GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);

    auto size = m_pDeviceResources->GetOutputSize();
    m_fontPos.x = float(size.right) / 2.f;
    m_fontPos.y = float(size.bottom) / 2.f;
}

void Renderer::BuildSpriteDataSize() {
    D3D12_VIEWPORT viewport = m_pDeviceResources->GetScreenViewport();
    D3D12_RECT size = m_pDeviceResources->GetOutputSize();
    m_pSpriteBatch->SetViewport(viewport);

    for (std::pair<const UINT, std::unique_ptr<SpriteData>>& pSpriteData : m_pSpriteData) {
        DirectX::XMUINT2 textureSize = DirectX::GetTextureSize(pSpriteData.second->pTexture.Get());

        // TODO: figure out the link between stretch and screen position.
        pSpriteData.second->StretchRect.left   = (size.right / 2); 
        pSpriteData.second->StretchRect.top    = (size.bottom / 2);
        pSpriteData.second->StretchRect.right  = (size.right / 2) + textureSize.x;
        pSpriteData.second->StretchRect.bottom = (size.bottom / 2) + textureSize.y;
    }
}
