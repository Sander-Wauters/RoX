#include "RoX/Renderer.h"

#include <CommonStates.h>
#include <ResourceUploadBatch.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <DirectXHelpers.h>
#include <RenderTargetState.h>

#include "Exceptions/ThrowIfFailed.h"
#include "Util/Logger.h"
#include "DebugDraw.h"
#include "ModelLoadAssimp.h"

Renderer::Renderer(Timer& timer, Camera& camera) noexcept : 
    m_timer(timer),
    m_camera(camera) 
{
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
    m_view = m_camera.GetView();
    m_proj = m_camera.GetProjection();

    for (std::pair<StaticGeometry::Base* const, std::unique_ptr<ObjectData::StaticGeometry>>& geo : m_staticGeoData) {
        geo.second->pEffect->SetView(m_view);
        geo.second->pEffect->SetProjection(m_proj);
    }
    for (std::pair<StaticGeometry::Base* const, std::unique_ptr<ObjectData::StaticGeometry>>& geo : m_instancedStaticGeoData) {
        geo.second->pEffect->SetView(m_view);
        geo.second->pEffect->SetProjection(m_proj);
    }

    m_pDebugDisplayEffect->SetView(m_view);
    m_pDebugDisplayEffect->SetProjection(m_proj);
}

void Renderer::Render() {
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    // Prepare the command list to render a new frame.
    if (m_msaaEnabled) {
        m_pDeviceResources->Prepare(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST);
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_pDeviceResources->GetMsaaRenderTarget(),
                D3D12_RESOURCE_STATE_RESOLVE_SOURCE, 
                D3D12_RESOURCE_STATE_RENDER_TARGET);
        pCommandList->ResourceBarrier(1, &barrier);
    } else {
        m_pDeviceResources->Prepare();
    }

    Clear();

    ID3D12DescriptorHeap* heaps[] = { m_pResourceDescriptors->Heap(), m_pStates->Heap() };
    pCommandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);

    // TEMP: debug drawing.
    m_pDebugDisplayEffect->SetWorld(m_world);
    m_pDebugDisplayEffect->Apply(pCommandList);
    m_pDebugDisplayPrimitiveBatch->Begin(pCommandList);
    DrawGrid(
            m_pDebugDisplayPrimitiveBatch.get(), 
            {{ 100.0f, 0.0f, 0.0f, 0.0f }},
            {{ 0.0f, 0.0f, 100.0f, 0.0f }},
            {{ 0.0f, 0.0f, 0.0f, 0.0f }},
            200, 200);
    m_pDebugDisplayPrimitiveBatch->End();

    RenderStaticGeometry();

    m_pSpriteBatch->Begin(pCommandList, DirectX::SpriteSortMode_FrontToBack);
    RenderSprites();
    RenderText();
    m_pSpriteBatch->End();

    // Show the new frame.
    if (m_msaaEnabled) {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_pDeviceResources->GetMsaaRenderTarget(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
        pCommandList->ResourceBarrier(1, &barrier);
        pCommandList->ResolveSubresource(m_pDeviceResources->GetRenderTarget(),
                0, m_pDeviceResources->GetMsaaRenderTarget(), 
                0, m_pDeviceResources->GetBackBufferFormat());

        m_pDeviceResources->Present(D3D12_RESOURCE_STATE_RESOLVE_DEST);
    } else {
        m_pDeviceResources->Present();
    }

    m_pGraphicsMemory->Commit(m_pDeviceResources->GetCommandQueue());
}

void Renderer::Add(Sprite* pSprite) {
    std::unique_ptr<ObjectData::Sprite> pSpriteData = std::make_unique<ObjectData::Sprite>();
    pSpriteData->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
    m_spriteData[pSprite] = std::move(pSpriteData);
}

void Renderer::Add(Text* pText) {
    std::unique_ptr<ObjectData::Text> pTextData = std::make_unique<ObjectData::Text>();
    pTextData->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
    m_textData[pText] = std::move(pTextData);
}

void Renderer::Add(StaticGeometry::Base* pStaticGeo) {
    std::unique_ptr<ObjectData::StaticGeometry> pStaticGeoData = std::make_unique<ObjectData::StaticGeometry>();

    if (pStaticGeo->pTexture->TextureFilePath != L"" && 
            m_textures.find(pStaticGeo->pTexture) == m_textures.end()) {
        std::unique_ptr<ObjectData::Texture> pTexture = std::make_unique<ObjectData::Texture>();
        pTexture->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
        m_textures[pStaticGeo->pTexture] = std::move(pTexture); 
    }
    if (pStaticGeo->pTexture->NormalMapFilePath != L"" &&
            m_normalMaps.find(pStaticGeo->pTexture) == m_normalMaps.end()) {
        std::unique_ptr<ObjectData::Texture> pNormalMap = std::make_unique<ObjectData::Texture>();
        pNormalMap->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
        m_normalMaps[pStaticGeo->pTexture] = std::move(pNormalMap); 
    }
    if (pStaticGeo->pTexture->SpecularMapFilePath != L"" &&
            m_specularMaps.find(pStaticGeo->pTexture) == m_specularMaps.end()) {
        std::unique_ptr<ObjectData::Texture> pSpecularMap = std::make_unique<ObjectData::Texture>();
        pSpecularMap->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
        m_specularMaps[pStaticGeo->pTexture] = std::move(pSpecularMap); 
    }

    if (pStaticGeo->Instanced)
        m_instancedStaticGeoData[pStaticGeo] = std::move(pStaticGeoData);
    else
        m_staticGeoData[pStaticGeo] = std::move(pStaticGeoData);
}

void Renderer::OnDeviceLost() {
    // Add Direct3D resource cleanup here.
    m_pDeviceResources->HandleDeviceLost();
    m_pGraphicsMemory.reset();

    m_pResourceDescriptors.reset();
    m_pStates.reset();
    m_pSpriteBatch.reset();

    for (std::pair<Sprite* const, std::unique_ptr<ObjectData::Sprite>>& sprite : m_spriteData) {
        sprite.second->pTexture.Reset();
    }
    for (std::pair<Text* const, std::unique_ptr<ObjectData::Text>>& text : m_textData) {
        text.second->pSpriteFont.reset();
    }
    for (std::pair<StaticGeometry::Base* const, std::unique_ptr<ObjectData::StaticGeometry>>& geo : m_staticGeoData) {
        geo.second->pGeometricPrimitive.reset();
        geo.second->pEffect.reset();
    }
    for (std::pair<Texture* const, std::unique_ptr<ObjectData::Texture>>& texture : m_textures) {
        texture.second->pTexture.Reset();
    }
    for (std::pair<Texture* const, std::unique_ptr<ObjectData::Texture>>& normal : m_normalMaps) {
        normal.second->pTexture.Reset();
    }
    for (std::pair<Texture* const, std::unique_ptr<ObjectData::Texture>>& specular : m_specularMaps) {
        specular.second->pTexture.Reset();
    }

    // TEMP: debug drawing.
    m_pDebugDisplayEffect.reset();
    m_pDebugDisplayPrimitiveBatch.reset();
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
    m_camera.SetFrustum(
            m_camera.GetFovY(), 
            (float)width / height, 
            m_camera.GetNearZ(), 
            m_camera.GetFarZ());
}

void Renderer::SetMsaa(bool state) noexcept {
    if (m_msaaEnabled == state)
        return;

    m_msaaEnabled = state;
    CreateRenderTargetDependentResources();
}

bool Renderer::MsaaEnabled() const noexcept {
    return m_msaaEnabled;
}

void Renderer::Clear() {
    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    CD3DX12_CPU_DESCRIPTOR_HANDLE const rtvDescriptor = m_msaaEnabled ? m_pDeviceResources->GetMsaaRenderTargetView() : m_pDeviceResources->GetRenderTargetView();
    CD3DX12_CPU_DESCRIPTOR_HANDLE const dsvDescriptor = m_msaaEnabled ? m_pDeviceResources->GetMsaaDepthStencilView() : m_pDeviceResources->GetDepthStencilView();
    pCommandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    pCommandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::CornflowerBlue, 0, nullptr);
    pCommandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    D3D12_VIEWPORT const viewport = m_pDeviceResources->GetScreenViewport();
    D3D12_RECT const scissorRect = m_pDeviceResources->GetScissorRect();
    pCommandList->RSSetViewports(1, &viewport);
    pCommandList->RSSetScissorRects(1, &scissorRect);
}

void Renderer::RenderSprites() {
    for (std::pair<Sprite* const, std::unique_ptr<ObjectData::Sprite>>& sprite : m_spriteData) {
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
    for (std::pair<Text* const, std::unique_ptr<ObjectData::Text>>& text : m_textData) {
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

void Renderer::RenderStaticGeometry() {
    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    for (std::pair<StaticGeometry::Base* const, std::unique_ptr<ObjectData::StaticGeometry>>& geo : m_staticGeoData) {
        geo.second->pEffect->SetWorld(geo.first->World);
        geo.second->pEffect->Apply(pCommandList);
        geo.second->pGeometricPrimitive->Draw(pCommandList);
    } 

    for (std::pair<StaticGeometry::Base* const, std::unique_ptr<ObjectData::StaticGeometry>>& geo : m_instancedStaticGeoData) {
        const size_t instBytes = geo.first->Instances.size() * sizeof(DirectX::XMFLOAT3X4);
        DirectX::GraphicsResource inst = m_pGraphicsMemory->Allocate(instBytes);
        memcpy(inst.Memory(), geo.first->Instances.data(), instBytes);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferInst = {};
        vertexBufferInst.BufferLocation = inst.GpuAddress();
        vertexBufferInst.SizeInBytes = static_cast<UINT>(instBytes);
        vertexBufferInst.StrideInBytes = sizeof(DirectX::XMFLOAT3X4);
        pCommandList->IASetVertexBuffers(1, 1, &vertexBufferInst);

        geo.second->pEffect->Apply(pCommandList);
        geo.second->pGeometricPrimitive->DrawInstanced(pCommandList, geo.first->Instances.size());
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
            m_spriteData.size() + m_textData.size() + 
            m_textures.size() + m_normalMaps.size() + 
            m_specularMaps.size());
    m_pStates = std::make_unique<DirectX::CommonStates>(pDevice);

    CreateRenderTargetDependentResources();
}

void Renderer::CreateRenderTargetDependentResources() {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    DirectX::RenderTargetState rtState(
            m_pDeviceResources->GetBackBufferFormat(), 
            m_pDeviceResources->GetDepthBufferFormat());
    if (m_msaaEnabled) {
        rtState.sampleDesc.Count = DeviceResources::MSAA_COUNT;
        rtState.sampleDesc.Quality = DeviceResources::MSAA_QUALITY;
    }

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    BuildSpriteDataResources(resourceUploadBatch);
    BuildTextDataResources(resourceUploadBatch);
    BuildTextureDataResources(resourceUploadBatch);
    BuildStaticGeoDataResources(false, rtState, resourceUploadBatch);
    BuildStaticGeoDataResources(true, rtState, resourceUploadBatch);

    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_pDeviceResources->GetCommandQueue());
    uploadResourceFinished.wait();

    BuildDebugDisplayResources(rtState);
}

void Renderer::BuildSpriteDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    for (std::pair<Sprite* const, std::unique_ptr<ObjectData::Sprite>>& sprite : m_spriteData) {
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

    for (std::pair<Text* const, std::unique_ptr<ObjectData::Text>>& text : m_textData) {
        std::unique_ptr<DirectX::SpriteFont> pFont = std::make_unique<DirectX::SpriteFont>(
                pDevice, 
                resourceUploadBatch,
                text.first->FilePath.c_str(),
                m_pResourceDescriptors->GetCpuHandle(text.second->DescriptorHeapIndex),
                m_pResourceDescriptors->GetGpuHandle(text.second->DescriptorHeapIndex));

        text.second->pSpriteFont = std::move(pFont);
    }
}

void Renderer::BuildTextureDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    for (std::pair<Texture* const, std::unique_ptr<ObjectData::Texture>>& texture : m_textures) {
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(
                    pDevice, resourceUploadBatch, 
                    texture.first->TextureFilePath.c_str(),
                    texture.second->pTexture.ReleaseAndGetAddressOf()));
        DirectX::CreateShaderResourceView(pDevice, texture.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(texture.second->DescriptorHeapIndex));
    }
    for (std::pair<Texture* const, std::unique_ptr<ObjectData::Texture>>& normalMap : m_normalMaps) {
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(
                    pDevice, resourceUploadBatch, 
                    normalMap.first->NormalMapFilePath.c_str(),
                    normalMap.second->pTexture.ReleaseAndGetAddressOf()));
        DirectX::CreateShaderResourceView(pDevice, normalMap.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(normalMap.second->DescriptorHeapIndex));
    }
    for (std::pair<Texture* const, std::unique_ptr<ObjectData::Texture>>& specularMap : m_specularMaps) {
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(
                    pDevice, resourceUploadBatch, 
                    specularMap.first->SpecularMapFilePath.c_str(),
                    specularMap.second->pTexture.ReleaseAndGetAddressOf()));
        DirectX::CreateShaderResourceView(pDevice, specularMap.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(specularMap.second->DescriptorHeapIndex));
    }
}

void Renderer::BuildStaticGeoDataResources(bool instanced, DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();


    const D3D12_INPUT_ELEMENT_DESC inputElements[] = {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
    };

    const D3D12_INPUT_LAYOUT_DESC layout = { inputElements, static_cast<UINT>(std::size(inputElements)) };

    DirectX::EffectPipelineStateDescription pd(
            instanced ? &layout : &DirectX::GeometricPrimitive::VertexType::InputLayout,
            DirectX::CommonStates::Opaque,
            DirectX::CommonStates::DepthDefault,
            DirectX::CommonStates::CullCounterClockwise,
            renderTargetState);

    for (std::pair<StaticGeometry::Base* const, std::unique_ptr<ObjectData::StaticGeometry>>& geo : instanced ? m_instancedStaticGeoData : m_staticGeoData) {
        if (StaticGeometry::Cube* p = dynamic_cast<StaticGeometry::Cube*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateCube(p->Size, false); 
        else if (StaticGeometry::Box* p = dynamic_cast<StaticGeometry::Box*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateBox(p->Size, false, p->InvertNormal);
        else if (StaticGeometry::Sphere* p = dynamic_cast<StaticGeometry::Sphere*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateSphere(p->Diameter, p->Tessellation, false, p->InvertNormal);
        else if (StaticGeometry::GeoSphere* p = dynamic_cast<StaticGeometry::GeoSphere*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateGeoSphere(p->Diameter, p->Tessellation, false);
        else if (StaticGeometry::Cylinder* p = dynamic_cast<StaticGeometry::Cylinder*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateCylinder(p->Height, p->Diameter, p->Tessellation, false);
        else if (StaticGeometry::Cone* p = dynamic_cast<StaticGeometry::Cone*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateCone(p->Diameter, p->Height, p->Tessellation, false);
        else if (StaticGeometry::Torus* p = dynamic_cast<StaticGeometry::Torus*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateTorus(p->Diameter, p->Thickness, p->Tessellation, false);
        else if (StaticGeometry::Tetrahedron* p = dynamic_cast<StaticGeometry::Tetrahedron*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateTetrahedron(p->Size, false);
        else if (StaticGeometry::Octahedron* p = dynamic_cast<StaticGeometry::Octahedron*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateOctahedron(p->Size, false);
        else if (StaticGeometry::Dodecahedron* p = dynamic_cast<StaticGeometry::Dodecahedron*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateDodecahedron(p->Size, false);
        else if (StaticGeometry::Icosahedron* p = dynamic_cast<StaticGeometry::Icosahedron*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateIcosahedron(p->Size, false);
        else if (StaticGeometry::Custom* p = dynamic_cast<StaticGeometry::Custom*>(geo.first)) 
            geo.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateCustom(p->Vertices, p->Indices);
        else
            throw std::runtime_error("Error downcasting StaticGeometry");

        // Updload to dedicated video memory for better performance.
        geo.second->pGeometricPrimitive->LoadStaticBuffers(pDevice, resourceUploadBatch);

        geo.second->pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice,
                instanced ? DirectX::EffectFlags::Instancing : DirectX::EffectFlags::Lighting, pd);
        geo.second->pEffect->EnableDefaultLighting();
        geo.second->pEffect->SetTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textures.at(geo.first->pTexture)->DescriptorHeapIndex),
                m_pStates->AnisotropicWrap());
        geo.second->pEffect->SetNormalTexture(
                m_pResourceDescriptors->GetGpuHandle(m_normalMaps.at(geo.first->pTexture)->DescriptorHeapIndex));
    }
}

void Renderer::BuildDebugDisplayResources(DirectX::RenderTargetState& renderTargetState) {
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();

    m_pDebugDisplayPrimitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(pDevice);

    CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE, FALSE,
            D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
            D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, TRUE, FALSE,
            0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

    DirectX::EffectPipelineStateDescription pd(
            &DirectX::VertexPositionColor::InputLayout,
            DirectX::CommonStates::Opaque,
            DirectX::CommonStates::DepthDefault,
            m_msaaEnabled ? rastDesc : DirectX::CommonStates::CullNone,
            renderTargetState,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    m_pDebugDisplayEffect = std::make_unique<DirectX::BasicEffect>(pDevice, DirectX::EffectFlags::VertexColor, pd);

    m_world = DirectX::SimpleMath::Matrix::Identity;
}

void Renderer::CreateWindowSizeDependentResources() {
    D3D12_VIEWPORT viewport = m_pDeviceResources->GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}

