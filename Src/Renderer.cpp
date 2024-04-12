#include "RoX/Renderer.h"

#include <ResourceUploadBatch.h>
#include <DescriptorHeap.h>
#include <CommonStates.h>
#include <RenderTargetState.h>
#include <SpriteBatch.h>
#include <DirectXHelpers.h>

#include "Util/pch.h"

#include "DebugDraw.h"
#include "IDeviceObserver.h"
#include "DeviceResources.h"
#include "DeviceDataBuilder.h"


class Renderer::Impl : public IDeviceObserver {
    public:
        Impl(Renderer* pOwner) noexcept;
        ~Impl() noexcept;
    
        Impl(Impl&&) = default;
        Impl& operator= (Impl&&) = default;

        Impl(Impl const&) = delete;
        Impl& operator= (Impl const&) = delete;

        void Initialize(HWND window, int width, int height);
        void Load(Scene& scene);

        void Update();
        void Render();

        void OnDeviceLost() override;
        void OnDeviceRestored() override;
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowMoved();
        void OnDisplayChange();
        void OnWindowSizeChanged(int width, int height);

        void SetMsaa(bool state) noexcept;
        bool IsMsaaEnabled() const noexcept;

    private:
        void Clear();

        void RenderMeshes();
        void RenderOutlines();
        void RenderSprites();
        void RenderText();

        void CreateDeviceDependentResources();
        void CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateWindowSizeDependentResources();

    private:
        Renderer* m_pOwner;
        std::unique_ptr<DeviceResources> m_pDeviceResources;
        std::unique_ptr<DirectX::GraphicsMemory> m_pGraphicsMemory;
        std::unique_ptr<DeviceDataBuilder> m_pDeviceDataBuilder;

        bool m_msaaEnabled;
        
};

Renderer::Impl::Impl(Renderer* pOwner) 
    noexcept : m_pOwner(pOwner),
    m_pDeviceResources(nullptr),
    m_pGraphicsMemory(nullptr),
    m_pDeviceDataBuilder(nullptr),
    m_msaaEnabled(false)
{
    m_pDeviceResources = std::make_unique<DeviceResources>();
    m_pDeviceResources->RegisterDeviceObserver(this);
}

Renderer::Impl::~Impl() noexcept {
    if (m_pDeviceResources)
        m_pDeviceResources->WaitForGpu();
}
void Renderer::Impl::Initialize(HWND window, int width, int height) {
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    m_pDeviceResources->CreateWindowSizeDependentResources();
}

void Renderer::Impl::Load(Scene& scene) {
    m_pDeviceDataBuilder = std::make_unique<DeviceDataBuilder>(scene, *m_pDeviceResources.get());
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void Renderer::Impl::Update() {
    if (m_pDeviceDataBuilder)
        m_pDeviceDataBuilder->Update();
}

void Renderer::Impl::Render() {
    // Don't try to render anything before the first Update
    // and before a Scene is loaded.
    if (m_pOwner->m_timer.GetFrameCount() == 0 || !m_pDeviceDataBuilder)
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

    // Only set the descriptor heap if there are resources.
    if (m_pDeviceDataBuilder->GetResourceDescriptorCount() > 0) {
        ID3D12DescriptorHeap* heaps[] = { 
            m_pDeviceDataBuilder->GetDescriptorHeap()->Heap(), 
            m_pDeviceDataBuilder->GetStates()->Heap() 
        };
        pCommandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);
    }

    if (m_pDeviceDataBuilder->GetMaterialData().size() > 0) 
        RenderMeshes();

    RenderOutlines();

    // Sprites can only be drawn if the descriptor heaps contains there resources.
    if (m_pDeviceDataBuilder->GetResourceDescriptorCount() > 0) {
        DirectX::SpriteBatch* pSpriteBatch = m_pDeviceDataBuilder->GetSpriteBatch();
        pSpriteBatch->Begin(pCommandList, DirectX::SpriteSortMode_FrontToBack);
        RenderSprites();
        RenderText();
        pSpriteBatch->End();
    }

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

void Renderer::Impl::OnDeviceLost() {
    m_pGraphicsMemory.reset();
    m_pDeviceDataBuilder->OnDeviceLost();
}

void Renderer::Impl::OnDeviceRestored() {
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void Renderer::Impl::OnActivated() {
    // Game is becoming active window.
}

void Renderer::Impl::OnDeactivated() {
    // Game is becoming background window.
}

void Renderer::Impl::OnSuspending() {
    // Game is being power-suspended (or minimized).
}

void Renderer::Impl::OnResuming() {
    // Game is being power-resumed (or returning from minimize).
    m_pOwner->m_timer.ResetElapsedTime();
}

void Renderer::Impl::OnWindowMoved() {
    RECT const r = m_pDeviceResources->GetOutputSize();
    m_pDeviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Renderer::Impl::OnDisplayChange() {
    m_pDeviceResources->UpdateColorSpace();
}

void Renderer::Impl::OnWindowSizeChanged(int width, int height) {
    // Game window is being resized.
    if (!m_pDeviceResources->WindowSizeChanged(width, height))
        return;
    CreateWindowSizeDependentResources();
}

void Renderer::Impl::SetMsaa(bool state) noexcept {
    if (m_msaaEnabled == state)
        return;

    m_msaaEnabled = state;

    m_pDeviceResources->WaitForGpu();
    ID3D12Device* pDevice = m_pDeviceResources->GetDevice();
    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    CreateRenderTargetDependentResources(resourceUploadBatch);
    CreateWindowSizeDependentResources();

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_pDeviceResources->GetCommandQueue());
    uploadResourceFinished.wait();
}

bool Renderer::Impl::IsMsaaEnabled() const noexcept {
    return m_msaaEnabled;
}

void Renderer::Impl::Clear() {
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

void Renderer::Impl::RenderMeshes() {
    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    for (const std::pair<Mesh* const, std::unique_ptr<DeviceData::Mesh>>& mesh : m_pDeviceDataBuilder->GetMeshData()) {
        if (!mesh.first->IsVisible())
            continue;

        const size_t instBytes = (mesh.first->GetInstances().size() - mesh.first->GetCulled()) * sizeof(DirectX::XMFLOAT3X4);
        DirectX::GraphicsResource inst = m_pGraphicsMemory->Allocate(instBytes);
        memcpy(inst.Memory(), mesh.first->GetInstances().data(), instBytes);

        D3D12_VERTEX_BUFFER_VIEW vertexBufferInst = {};
        vertexBufferInst.BufferLocation = inst.GpuAddress();
        vertexBufferInst.SizeInBytes = static_cast<UINT>(instBytes);
        vertexBufferInst.StrideInBytes = sizeof(DirectX::XMFLOAT3X4);
        pCommandList->IASetVertexBuffers(1, 1, &vertexBufferInst);

        mesh.second->pMaterialData->pEffect->Apply(pCommandList);
        mesh.second->pGeometricPrimitive->DrawInstanced(pCommandList, mesh.first->GetInstances().size());
    }
}

void Renderer::Impl::RenderOutlines() {
    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* pOutlineBatch = m_pDeviceDataBuilder->GetOutlineBatch();
    pOutlineBatch->Begin(pCommandList);

    DirectX::BasicEffect* pOutlineEffect = m_pDeviceDataBuilder->GetOutlineEffect();
    pOutlineEffect->SetWorld(DirectX::XMMatrixIdentity());
    pOutlineEffect->Apply(pCommandList);

    for (const std::pair<std::string const, std::shared_ptr<Outline::Base>>& outline : m_pDeviceDataBuilder->GetScene().GetOutlines()) {
        if (!outline.second->IsVisible())
            continue;

        if (std::shared_ptr<Outline::BoundingBody<DirectX::BoundingBox>> p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingBox>>(outline.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (std::shared_ptr<Outline::BoundingBody<DirectX::BoundingFrustum>> p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingFrustum>>(outline.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (std::shared_ptr<Outline::BoundingBody<DirectX::BoundingOrientedBox>> p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingOrientedBox>>(outline.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (std::shared_ptr<Outline::BoundingBody<DirectX::BoundingSphere>> p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingSphere>>(outline.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (std::shared_ptr<Outline::Grid> p = std::dynamic_pointer_cast<Outline::Grid>(outline.second))
            DrawGrid(pOutlineBatch, p->GetXAxis(), p->GetYAxis(), p->GetOrigin(), p->GetXDivsions(), p->GetYDivsions(), p->GetColor());
        else if (std::shared_ptr<Outline::Ring> p = std::dynamic_pointer_cast<Outline::Ring>(outline.second))
            DrawRing(pOutlineBatch, p->GetOrigin(), p->GetMajorAxis(), p->GetMinorAxis(), p->GetColor());
        else if (std::shared_ptr<Outline::Ray> p = std::dynamic_pointer_cast<Outline::Ray>(outline.second)) 
            DrawRay(pOutlineBatch, p->GetOrigin(), p->GetDirection(), p->IsNormalized(), p->GetColor());
        else if (std::shared_ptr<Outline::Triangle> p = std::dynamic_pointer_cast<Outline::Triangle>(outline.second)) 
            DrawTriangle(pOutlineBatch, p->GetPointA(), p->GetPointB(), p->GetPointC(), p->GetColor());
        else if (std::shared_ptr<Outline::Quad> p = std::dynamic_pointer_cast<Outline::Quad>(outline.second)) 
            DrawQuad(pOutlineBatch, p->GetPointA(), p->GetPointB(), p->GetPointC(), p->GetPointD(), p->GetColor());
    }
    pOutlineBatch->End();
}

void Renderer::Impl::RenderSprites() {
    DirectX::SpriteBatch* pSpriteBatch = m_pDeviceDataBuilder->GetSpriteBatch();

    for (const std::pair<Sprite* const, std::unique_ptr<DeviceData::Texture>>& sprite : m_pDeviceDataBuilder->GetSpriteData()) {
        if (!sprite.first->IsVisible())
            continue;

        DirectX::XMUINT2 textureSize = DirectX::GetTextureSize(sprite.second->pTexture.Get());

        DirectX::XMFLOAT2 origin = { -sprite.first->GetOrigin().x, -sprite.first->GetOrigin().y };
        origin.x += (float)textureSize.x / 2;
        origin.y += (float)textureSize.y / 2;

        DirectX::XMFLOAT2 offset;
        offset.x = (float)textureSize.x / 2;
        offset.y = (float)textureSize.y / 2;
        offset.x *= sprite.first->GetScale().x;
        offset.y *= sprite.first->GetScale().y;
        offset.x += sprite.first->GetOffset().x;
        offset.y += sprite.first->GetOffset().y;

        pSpriteBatch->Draw(
                m_pDeviceDataBuilder->GetDescriptorHeap()->GetGpuHandle(sprite.second->DescriptorHeapIndex),
                textureSize,
                offset,
                nullptr,
                sprite.first->GetColor(),
                sprite.first->GetAngle(),
                origin,
                sprite.first->GetScale(),
                DirectX::SpriteEffects_None,
                sprite.first->GetLayer());
    }
}

void Renderer::Impl::RenderText() {
    DirectX::SpriteBatch* pSpriteBatch = m_pDeviceDataBuilder->GetSpriteBatch();

    for (const std::pair<Text* const, std::unique_ptr<DeviceData::Text>>& text : m_pDeviceDataBuilder->GetTextData()) {
        text.second->pSpriteFont->DrawString(
                pSpriteBatch,
                text.first->GetContent().c_str(),
                text.first->GetOffset(),
                text.first->GetColor(),
                text.first->GetAngle(),
                { -text.first->GetOrigin().x, -text.first->GetOrigin().y },
                text.first->GetScale(),
                DirectX::SpriteEffects_None,
                text.first->GetLayer());
    }
}

void Renderer::Impl::CreateDeviceDependentResources() {
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
    if (m_pDeviceDataBuilder)
        m_pDeviceDataBuilder->BuildDeviceDependentResources(m_msaaEnabled);
}

void Renderer::Impl::CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    if (m_pDeviceDataBuilder)
        m_pDeviceDataBuilder->BuildRenderTargetDependentResources(resourceUploadBatch, m_msaaEnabled);
}

void Renderer::Impl::CreateWindowSizeDependentResources() {
    if (m_pDeviceDataBuilder)
        m_pDeviceDataBuilder->BuildWindowSizeDependentResources();
}

Renderer::Renderer(Timer& timer) 
    noexcept : m_timer(timer), 
    m_pImpl(std::make_unique<Impl>(this)) 
{} 
Renderer::~Renderer() noexcept {}
Renderer::Renderer(Renderer&& moveFrom) noexcept : m_timer(moveFrom.m_timer), m_pImpl(std::make_unique<Impl>(this)) {}
void Renderer::Initialize(HWND window, int width, int height) { m_pImpl->Initialize(window, width, height); }
void Renderer::Load(Scene& scene) { m_pImpl->Load(scene); }
void Renderer::Update() { m_pImpl->Update(); }
void Renderer::Render() { m_pImpl->Render(); }
void Renderer::OnActivated() { m_pImpl->OnActivated(); }
void Renderer::OnDeactivated() { m_pImpl->OnDeactivated(); }
void Renderer::OnSuspending() { m_pImpl->OnSuspending(); }
void Renderer::OnResuming() { m_pImpl->OnResuming(); }
void Renderer::OnWindowMoved() { m_pImpl->OnWindowMoved(); }
void Renderer::OnDisplayChange() { m_pImpl->OnDisplayChange(); }
void Renderer::OnWindowSizeChanged(int width, int height) { m_pImpl->OnWindowSizeChanged(width, height); }
void Renderer::SetMsaa(bool state) noexcept { m_pImpl->SetMsaa(state); }
bool Renderer::IsMsaaEnabled() const noexcept { return m_pImpl->IsMsaaEnabled(); }

