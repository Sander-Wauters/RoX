#include "RoX/Renderer.h"

#include <ResourceUploadBatch.h>
#include <DescriptorHeap.h>
#include <CommonStates.h>
#include <RenderTargetState.h>
#include <SpriteBatch.h>
#include <DirectXHelpers.h>

#include <ImGui/imgui.h>
#include <ImGuiBackends/imgui_impl_dx12.h>
#include <ImGuiBackends/imgui_impl_win32.h>

#include "Util/pch.h"

#include "DebugDraw.h"
#include "IDeviceObserver.h"
#include "DeviceResources.h"
#include "DeviceResourceData.h"


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
        void Render(const std::function<void()>& renderImGui);

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
        void RenderOldMeshes();
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
        std::unique_ptr<DeviceResourceData> m_pDeviceResourceData;

        bool m_msaaEnabled;
        
};

Renderer::Impl::Impl(Renderer* pOwner) 
    noexcept : m_pOwner(pOwner),
    m_pDeviceResources(nullptr),
    m_pGraphicsMemory(nullptr),
    m_pDeviceResourceData(nullptr),
    m_msaaEnabled(false)
{
    m_pDeviceResources = std::make_unique<DeviceResources>();
    m_pDeviceResources->RegisterDeviceObserver(this);
}

Renderer::Impl::~Impl() noexcept {
    if (m_pDeviceResources)
        m_pDeviceResources->WaitForGpu();

    ImGui_ImplDX12_Shutdown();
    ImGui::DestroyContext();
}
void Renderer::Impl::Initialize(HWND window, int width, int height) {
    m_pDeviceResources->SetWindow(window, width, height);
    m_pDeviceResources->CreateDeviceResources();
    m_pDeviceResources->CreateWindowSizeDependentResources();

    m_pGraphicsMemory = std::make_unique<DirectX::GraphicsMemory>(m_pDeviceResources->GetDevice());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplWin32_Init(window);
}

void Renderer::Impl::Load(Scene& scene) {
    m_pDeviceResourceData = std::make_unique<DeviceResourceData>(scene, *m_pDeviceResources.get());
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
    ImGui_ImplDX12_Init(m_pDeviceResources->GetDevice(), 
            m_pDeviceResources->GetBackBufferCount(),
            m_pDeviceResources->GetBackBufferFormat(),
            m_pDeviceResourceData->GetDescriptorHeap()->Heap(),
            m_pDeviceResourceData->GetImGuiCpuDescHandle(),
            m_pDeviceResourceData->GetImGuiGpuDescHandle());
}

void Renderer::Impl::Update() {
    if (m_pDeviceResourceData) 
        m_pDeviceResourceData->Update();
}

void Renderer::Impl::Render(const std::function<void()>& renderImGui) {
    // Don't try to render anything before the first Update and before a Scene is loaded.
    if (m_pOwner->m_timer.GetFrameCount() == 0 || !m_pDeviceResourceData)
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

    if (m_pDeviceResourceData->HasTextures()) {
        ID3D12DescriptorHeap* heaps[] = { 
            m_pDeviceResourceData->GetDescriptorHeap()->Heap(), 
            m_pDeviceResourceData->GetStates()->Heap() 
        };
        pCommandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);
    }

    if (m_pDeviceResourceData->HasMaterials() && m_pDeviceResourceData->HasTextures())
        RenderMeshes();

    RenderOutlines();

    if (m_pDeviceResourceData->HasTextures()) {
        DirectX::SpriteBatch* pSpriteBatch = m_pDeviceResourceData->GetSpriteBatch();
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
        // ImGui doesn't support MSAA.
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        renderImGui();

        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
        m_pDeviceResources->Present();
    }

    m_pGraphicsMemory->Commit(m_pDeviceResources->GetCommandQueue());
}

void Renderer::Impl::OnDeviceLost() {
    m_pGraphicsMemory.reset();
    m_pDeviceResourceData->OnDeviceLost();
    ImGui_ImplDX12_InvalidateDeviceObjects();
}

void Renderer::Impl::OnDeviceRestored() {
    ImGui_ImplDX12_CreateDeviceObjects();
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
}

void Renderer::Impl::OnWindowMoved() {
    const RECT r = m_pDeviceResources->GetOutputSize();
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

    for (const ModelPair& modelPair : m_pDeviceResourceData->GetModelData()) {
        if (!modelPair.first->IsVisible())
            continue;
        if (modelPair.first->IsSkinned()) {
            modelPair.second->DrawSkinned(pCommandList, modelPair.first.get());
            continue;
        }

        for (std::uint64_t meshIndex = 0; meshIndex < modelPair.first->GetNumMeshes(); ++meshIndex) {
            IMesh* pMesh = modelPair.first->GetMeshes()[meshIndex].get();
            MeshDeviceData* pMeshData = modelPair.second->GetMeshes()[meshIndex];

            if (!pMesh->IsVisible())
                continue;

            pMeshData->PrepareForDraw(pCommandList);

            for (std::uint64_t submeshIndex = 0; submeshIndex < pMesh->GetNumSubmeshes(); ++submeshIndex) {
                Submesh* pSubmesh = pMesh->GetSubmeshes()[submeshIndex].get();
                SubmeshDeviceData* pSubmeshData = pMeshData->GetSubmeshes()[submeshIndex].get();

                if (!pSubmesh->IsVisible())
                    continue;

                DirectX::IEffect* pEffect = modelPair.second->GetEffects()[pSubmesh->GetMaterialIndex()]->get();

                std::uint32_t flags = pSubmesh->GetMaterial(*modelPair.first)->GetFlags();

                if (flags & RenderFlags::Effect::Instanced) {
                    const size_t instBytes = pSubmesh->GetNumVisibleInstances() * sizeof(DirectX::XMFLOAT3X4);
                    DirectX::GraphicsResource inst = m_pGraphicsMemory->Allocate(instBytes);
                    memcpy(inst.Memory(), pSubmesh->GetInstances().data(), instBytes);

                    D3D12_VERTEX_BUFFER_VIEW vertexBufferInst = {};
                    vertexBufferInst.BufferLocation = inst.GpuAddress();
                    vertexBufferInst.SizeInBytes = static_cast<UINT>(instBytes);
                    vertexBufferInst.StrideInBytes = sizeof(DirectX::XMFLOAT3X4);
                    pCommandList->IASetVertexBuffers(1, 1, &vertexBufferInst);

                    pEffect->Apply(pCommandList);
                    pSubmeshData->DrawInstanced(pCommandList, pSubmesh);
                } else {
                    DirectX::XMMATRIX world = DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0]);
                    if (pMesh->GetBoneIndex() != Bone::INVALID_INDEX && modelPair.first->GetBoneMatrices() != nullptr) 
                        world = DirectX::XMMatrixMultiply(modelPair.first->GetBoneMatrices()[pMesh->GetBoneIndex()], world);

                    auto iMatrices = dynamic_cast<DirectX::IEffectMatrices*>(pEffect);
                    if (iMatrices)
                        iMatrices->SetWorld(world);
                    pEffect->Apply(pCommandList);

                    pSubmeshData->Draw(pCommandList, pSubmesh);
                }
            }
        }
    }
}

void Renderer::Impl::RenderOutlines() {
    ID3D12GraphicsCommandList* pCommandList = m_pDeviceResources->GetCommandList();

    DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* pOutlineBatch = m_pDeviceResourceData->GetOutlineBatch();
    pOutlineBatch->Begin(pCommandList);

    DirectX::BasicEffect* pOutlineEffect = m_pDeviceResourceData->GetOutlineEffect();
    pOutlineEffect->SetWorld(DirectX::XMMatrixIdentity());
    pOutlineEffect->Apply(pCommandList);

    for (const std::pair<std::string const, std::shared_ptr<Outline::Base>>& outlinePair : m_pDeviceResourceData->GetScene().GetOutlines()) {
        if (!outlinePair.second->IsVisible())
            continue;

        if (auto p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingBox>>(outlinePair.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingFrustum>>(outlinePair.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingOrientedBox>>(outlinePair.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::BoundingBody<DirectX::BoundingSphere>>(outlinePair.second)) 
            Draw(pOutlineBatch, p->GetBounds(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::Grid>(outlinePair.second))
            DrawGrid(pOutlineBatch, p->GetXAxis(), p->GetYAxis(), p->GetOrigin(), p->GetXDivsions(), p->GetYDivsions(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::Ring>(outlinePair.second))
            DrawRing(pOutlineBatch, p->GetOrigin(), p->GetMajorAxis(), p->GetMinorAxis(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::Ray>(outlinePair.second)) 
            DrawRay(pOutlineBatch, p->GetOrigin(), p->GetDirection(), p->IsNormalized(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::Triangle>(outlinePair.second)) 
            DrawTriangle(pOutlineBatch, p->GetPointA(), p->GetPointB(), p->GetPointC(), p->GetColor());
        else if (auto p = std::dynamic_pointer_cast<Outline::Quad>(outlinePair.second)) 
            DrawQuad(pOutlineBatch, p->GetPointA(), p->GetPointB(), p->GetPointC(), p->GetPointD(), p->GetColor());
    }
    pOutlineBatch->End();
}

void Renderer::Impl::RenderSprites() {
    DirectX::SpriteBatch* pSpriteBatch = m_pDeviceResourceData->GetSpriteBatch();

    for (const SpritePair& spritePair : m_pDeviceResourceData->GetSpriteData()) {
        if (!spritePair.first->IsVisible())
            continue;

        DirectX::XMUINT2 textureSize = DirectX::GetTextureSize(spritePair.second->GetTexture().Get());

        DirectX::XMFLOAT2 origin = { -spritePair.first->GetOrigin().x, -spritePair.first->GetOrigin().y };
        origin.x += (float)textureSize.x / 2;
        origin.y += (float)textureSize.y / 2;

        DirectX::XMFLOAT2 offset;
        offset.x = (float)textureSize.x / 2;
        offset.y = (float)textureSize.y / 2;
        offset.x *= spritePair.first->GetScale().x;
        offset.y *= spritePair.first->GetScale().y;
        offset.x += spritePair.first->GetOffset().x;
        offset.y += spritePair.first->GetOffset().y;

        pSpriteBatch->Draw(
                m_pDeviceResourceData->GetDescriptorHeap()->GetGpuHandle(spritePair.second->GetHeapIndex()),
                textureSize,
                offset,
                nullptr,
                spritePair.first->GetColor(),
                spritePair.first->GetAngle(),
                origin,
                spritePair.first->GetScale(),
                DirectX::SpriteEffects_None,
                spritePair.first->GetLayer());
    }
}

void Renderer::Impl::RenderText() {
    DirectX::SpriteBatch* pSpriteBatch = m_pDeviceResourceData->GetSpriteBatch();

    for (const TextPair& textPair : m_pDeviceResourceData->GetTextData()) {
        textPair.second->GetSpriteFont()->DrawString(
                pSpriteBatch,
                textPair.first->GetContent().c_str(),
                textPair.first->GetOffset(),
                textPair.first->GetColor(),
                textPair.first->GetAngle(),
                { -textPair.first->GetOrigin().x, -textPair.first->GetOrigin().y },
                textPair.first->GetScale(),
                DirectX::SpriteEffects_None,
                textPair.first->GetLayer());
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

    if (m_pDeviceResourceData)
        m_pDeviceResourceData->BuildDeviceDependentResources(m_msaaEnabled);
}

void Renderer::Impl::CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    if (m_pDeviceResourceData)
        m_pDeviceResourceData->BuildRenderTargetDependentResources(resourceUploadBatch, m_msaaEnabled);
}

void Renderer::Impl::CreateWindowSizeDependentResources() {
    if (m_pDeviceResourceData)
        m_pDeviceResourceData->BuildWindowSizeDependentResources();
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
void Renderer::Render() { m_pImpl->Render([&](){}); }
void Renderer::Render(const std::function<void()>& renderImGui) { m_pImpl->Render(renderImGui); }
void Renderer::OnActivated() { m_pImpl->OnActivated(); }
void Renderer::OnDeactivated() { m_pImpl->OnDeactivated(); }
void Renderer::OnSuspending() { m_pImpl->OnSuspending(); }
void Renderer::OnResuming() { m_pImpl->OnResuming(); }
void Renderer::OnWindowMoved() { m_pImpl->OnWindowMoved(); }
void Renderer::OnDisplayChange() { m_pImpl->OnDisplayChange(); }
void Renderer::OnWindowSizeChanged(int width, int height) { m_pImpl->OnWindowSizeChanged(width, height); }
void Renderer::SetMsaa(bool state) noexcept { m_pImpl->SetMsaa(state); }
bool Renderer::IsMsaaEnabled() const noexcept { return m_pImpl->IsMsaaEnabled(); }

