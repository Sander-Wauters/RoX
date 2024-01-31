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

    RenderSprites();
    RenderText();

    m_pSpriteBatch->End();

    // Show the new frame.
    m_pDeviceResources->Present();
    m_pGraphicsMemory->Commit(m_pDeviceResources->GetCommandQueue());
}

void Renderer::AddSprite(Sprite* pSprite) {
    std::unique_ptr<SpriteData> pSpriteData = std::make_unique<SpriteData>();
    pSpriteData->DescriptorHeapIndex = m_nextSpriteDescriptorHeapIndex++;
    m_spriteData[pSprite] = std::move(pSpriteData);
}

void Renderer::AddText(Text* pText) {
    std::unique_ptr<TextData> pTextData = std::make_unique<TextData>();
    pTextData->DescriptorHeapIndex = m_nextSpriteDescriptorHeapIndex++;
    m_textData[pText] = std::move(pTextData);
}

void Renderer::OnDeviceLost() {
    // Add Direct3D resource cleanup here.
    m_pDeviceResources->HandleDeviceLost();
    m_pGraphicsMemory.reset();

    m_pResourceDescriptors.reset();
    m_pStates.reset();
    m_pSpriteBatch.reset();

    for (std::pair<Sprite* const, std::unique_ptr<SpriteData>>& sprite : m_spriteData) {
        sprite.second->pTexture.Reset();
    }
    for (std::pair<Text* const, std::unique_ptr<TextData>>& text : m_textData) {
        text.second->pSpriteFont.reset();
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
    for (std::pair<Sprite* const, std::unique_ptr<SpriteData>>& sprite : m_spriteData) {
        if (!sprite.first->Visible)
            continue;

        DirectX::SimpleMath::Vector2 origin = sprite.second->Origin;
        origin.x -= sprite.first->OriginOffsetX;
        origin.y -= sprite.first->OriginOffsetY;

        RECT stretchRect = sprite.second->StretchRect;
        stretchRect.right  += sprite.first->WidthStretch / 2;
        stretchRect.bottom += sprite.first->HeightStretch / 2;

        stretchRect.right  *= sprite.first->Scale;
        stretchRect.bottom *= sprite.first->Scale;

        stretchRect.left   += sprite.first->PositionX;
        stretchRect.top    += sprite.first->PositionY;
        stretchRect.right  += sprite.first->PositionX;
        stretchRect.bottom += sprite.first->PositionY;

        m_pSpriteBatch->Draw(
                m_pResourceDescriptors->GetGpuHandle(sprite.second->DescriptorHeapIndex),
                DirectX::GetTextureSize(sprite.second->pTexture.Get()),
                stretchRect,
                nullptr,
                sprite.second->Tint,
                sprite.first->Angle,
                origin,
                DirectX::SpriteEffects_None,
                sprite.first->Layer);
    }
}

void Renderer::RenderText() {
    for (std::pair<Text* const, std::unique_ptr<TextData>>& text : m_textData) {
        if (!text.first->Visible)
            continue;

        DirectX::SpriteFont* pFont = text.second->pSpriteFont.get();

        DirectX::SimpleMath::Vector2 origin = pFont->MeasureString(text.first->Content.c_str());
        origin /= 2.0f;
        origin.x += text.first->OriginOffsetX;
        origin.y += text.first->OriginOffsetY;

        DirectX::SimpleMath::Vector2 position = origin;
        position.x += text.first->PositionX;
        position.y += text.first->PositionY;

        pFont->DrawString(
                m_pSpriteBatch.get(), 
                text.first->Content.c_str(),
                position, 
                DirectX::Colors::Black, 
                text.first->Angle, 
                origin, 
                text.first->Scale, 
                DirectX::SpriteEffects_None, 
                text.first->Layer);
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
            m_spriteData.size() + m_textData.size());
    m_pStates = std::make_unique<DirectX::CommonStates>(pDevice);

    DirectX::RenderTargetState rtState(m_pDeviceResources->GetBackBufferFormat(), m_pDeviceResources->GetDepthBufferFormat());
    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);

    resourceUploadBatch.Begin();
    BuildSpriteDataResources(resourceUploadBatch);
    BuildTextDataResources(resourceUploadBatch);
    
    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);
        
    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_pDeviceResources->GetCommandQueue());
    uploadResourceFinished.wait();
}

void Renderer::BuildSpriteDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    for (std::pair<Sprite* const, std::unique_ptr<SpriteData>>& sprite : m_spriteData) {
        if (sprite.first->FilePath.size() < 5)
            throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "BuildSpriteDataResources");

        std::wstring fileExtension = sprite.first->FilePath.substr(sprite.first->FilePath.size() - 4);
        if (fileExtension == L".png") {
            ThrowIfFailed(DirectX::CreateWICTextureFromFile(
                        pDevice, resourceUploadBatch, sprite.first->FilePath.c_str(), sprite.second->pTexture.ReleaseAndGetAddressOf()));  
        } else if (fileExtension == L".dds") {
            ThrowIfFailed(DirectX::CreateDDSTextureFromFile(
                        pDevice, resourceUploadBatch, sprite.first->FilePath.c_str(), sprite.second->pTexture.ReleaseAndGetAddressOf()));
        } else
            throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "BuildSpriteDataResources");

        DirectX::CreateShaderResourceView(pDevice, sprite.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(sprite.second->DescriptorHeapIndex));

        DirectX::XMUINT2 textureSize = DirectX::GetTextureSize(sprite.second->pTexture.Get());
        DirectX::XMUINT2 textureSizeHalf = textureSize;
        textureSizeHalf.x /= 2;
        textureSizeHalf.y /= 2;

        sprite.second->Origin.x = textureSizeHalf.x;
        sprite.second->Origin.y = textureSizeHalf.y;

        sprite.second->StretchRect.left   = textureSizeHalf.x; 
        sprite.second->StretchRect.top    = textureSizeHalf.y;
        sprite.second->StretchRect.right  = textureSizeHalf.x + textureSize.x;
        sprite.second->StretchRect.bottom = textureSizeHalf.y + textureSize.y;
    }
}

void Renderer::BuildTextDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    for (std::pair<Text* const, std::unique_ptr<TextData>>& text : m_textData) {
        std::unique_ptr<DirectX::SpriteFont> pFont = std::make_unique<DirectX::SpriteFont>(
                pDevice, 
                resourceUploadBatch,
                text.first->FilePath.c_str(),
                m_pResourceDescriptors->GetCpuHandle(text.second->DescriptorHeapIndex),
                m_pResourceDescriptors->GetGpuHandle(text.second->DescriptorHeapIndex));

        text.second->pSpriteFont = std::move(pFont);
    }
}

void Renderer::CreateWindowSizeDependentResources() {
    BuildSpriteDataSize();
}

void Renderer::BuildSpriteDataSize() {
    D3D12_VIEWPORT viewport = m_pDeviceResources->GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}
