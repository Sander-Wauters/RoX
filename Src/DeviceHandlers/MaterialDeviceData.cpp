#include "MaterialDeviceData.h"

#include "RoX/VertexTypes.h"

MaterialDeviceData::MaterialDeviceData(
        DeviceResources& deviceResources, 
        DirectX::DescriptorHeap* pDescriptorHeap, 
        DirectX::CommonStates* pCommonStates,
        DirectX::RenderTargetState* pRtState,
        std::shared_ptr<TextureDeviceData> pDiffuseMap,
        std::shared_ptr<TextureDeviceData> pNormalMap,
        Material& material) 
    : m_deviceResources(deviceResources),
    m_pDescriptorHeap(pDescriptorHeap),
    m_pCommonStates(pCommonStates),
    m_pRtState(pRtState),
    m_flags(material.GetFlags()),
    m_pDiffuseMap(pDiffuseMap),
    m_pNormalMap(pNormalMap)
{
    m_deviceResources.Attach(this);
    CreateIEffect();
}

MaterialDeviceData::~MaterialDeviceData() noexcept {
    m_deviceResources.Detach(this);
}

void MaterialDeviceData::OnDeviceLost() {
    m_pIEffect.reset();
}

void MaterialDeviceData::OnDeviceRestored() {
    CreateIEffect();
}

void MaterialDeviceData::UpdateIEffect(DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Material& material) noexcept {
    if (auto pIMatrices = dynamic_cast<DirectX::IEffectMatrices*>(m_pIEffect.get())) {
        pIMatrices->SetView(view);
        pIMatrices->SetProjection(projection);
    }
    if (auto pNormal = dynamic_cast<DirectX::NormalMapEffect*>(m_pIEffect.get())) {
        pNormal->SetDiffuseColor(DirectX::XMLoadFloat4(&material.GetDiffuseColor()));
        pNormal->SetEmissiveColor(DirectX::XMLoadFloat4(&material.GetEmissiveColor()));
        pNormal->SetSpecularColor(DirectX::XMLoadFloat4(&material.GetSpecularColor()));
    }
}

void MaterialDeviceData::CreateRenderTargetDependentResources() {
    CreateIEffect();
}

void MaterialDeviceData::CreateIEffect() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    D3D12_INPUT_LAYOUT_DESC inputLayout = InputLayoutDesc(m_flags);
    DirectX::EffectPipelineStateDescription pd(
            &inputLayout,
            BlendDesc(m_flags),
            DepthStencilDesc(m_flags),
            RasterizerDesc(m_flags),
            *m_pRtState);

    if (m_flags & RenderFlags::Effect::Instanced) 
        m_pIEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Instancing, pd);
    else if (m_flags & RenderFlags::Effect::Skinned) 
        m_pIEffect = std::make_unique<DirectX::SkinnedNormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting, pd);
    else
        m_pIEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting | DirectX::EffectFlags::Texture, pd);

    auto pNormal = dynamic_cast<DirectX::NormalMapEffect*>(m_pIEffect.get());
    pNormal->EnableDefaultLighting();

    BindTexturesToIEffect();
}

void MaterialDeviceData::BindTexturesToIEffect() {
    auto pNormal = dynamic_cast<DirectX::NormalMapEffect*>(m_pIEffect.get());
    pNormal->SetTexture(m_pDescriptorHeap->GetGpuHandle(m_pDiffuseMap->GetHeapIndex()), SamplerDesc(m_flags));
    pNormal->SetNormalTexture(m_pDescriptorHeap->GetGpuHandle(m_pNormalMap->GetHeapIndex()));
}

D3D12_INPUT_LAYOUT_DESC MaterialDeviceData::InputLayoutDesc(std::uint32_t flags) const {
    if (flags & RenderFlags::Effect::Instanced)
        return VertexPositionNormalTexture::InputLayoutInstancing;
    if (flags & RenderFlags::Effect::Skinned)
        return VertexPositionNormalTextureSkinning::InputLayout;
    return VertexPositionNormalTexture::InputLayout;
}

D3D12_BLEND_DESC MaterialDeviceData::BlendDesc(std::uint32_t flags) const noexcept {
    if (flags & RenderFlags::BlendState::Opaque)
        return DirectX::CommonStates::Opaque;
    if (flags & RenderFlags::BlendState::AlphaBlend)
        return DirectX::CommonStates::AlphaBlend;
    if (flags & RenderFlags::BlendState::Additive)
        return DirectX::CommonStates::Additive;
    if (flags & RenderFlags::BlendState::NonPremultiplied)
        return DirectX::CommonStates::NonPremultiplied;
    return DirectX::CommonStates::Opaque;
}

D3D12_DEPTH_STENCIL_DESC MaterialDeviceData::DepthStencilDesc(std::uint32_t flags) const noexcept {
    if (flags & RenderFlags::DepthStencilState::None)    
        return DirectX::CommonStates::DepthNone;
    if (flags & RenderFlags::DepthStencilState::Default)    
        return DirectX::CommonStates::DepthDefault;
    if (flags & RenderFlags::DepthStencilState::Read)    
        return DirectX::CommonStates::DepthRead;
    if (flags & RenderFlags::DepthStencilState::ReverseZ)    
        return DirectX::CommonStates::DepthReverseZ;
    if (flags & RenderFlags::DepthStencilState::ReadReverseZ)    
        return DirectX::CommonStates::DepthReadReverseZ;
    return DirectX::CommonStates::DepthDefault;
}

D3D12_RASTERIZER_DESC MaterialDeviceData::RasterizerDesc(std::uint32_t flags) const noexcept {
    if (flags & RenderFlags::RasterizerState::CullNone)
        return DirectX::CommonStates::CullNone;
    if (flags & RenderFlags::RasterizerState::CullClockwise)
        return DirectX::CommonStates::CullClockwise;
    if (flags & RenderFlags::RasterizerState::CullCounterClockwise)
        return DirectX::CommonStates::CullCounterClockwise;
    if (flags & RenderFlags::RasterizerState::Wireframe)
        return DirectX::CommonStates::Wireframe;
    return DirectX::CommonStates::CullCounterClockwise;
}

D3D12_GPU_DESCRIPTOR_HANDLE MaterialDeviceData::SamplerDesc(std::uint32_t flags) const noexcept {
    if (flags & RenderFlags::SamplerState::PointWrap)
        return m_pCommonStates->PointWrap();
    if (flags & RenderFlags::SamplerState::PointClamp)
        return m_pCommonStates->PointClamp();
    if (flags & RenderFlags::SamplerState::LinearWrap)
        return m_pCommonStates->LinearWrap();
    if (flags & RenderFlags::SamplerState::LinearClamp)
        return m_pCommonStates->LinearClamp();
    if (flags & RenderFlags::SamplerState::AnisotropicWrap)
        return m_pCommonStates->AnisotropicWrap();
    if (flags & RenderFlags::SamplerState::AnisotropicClamp)
        return m_pCommonStates->AnisotropicClamp();
    return m_pCommonStates->AnisotropicWrap();
}

DirectX::IEffect* MaterialDeviceData::GetIEffect() {
    return m_pIEffect.get();
}

void MaterialDeviceData::SetDescriptorHeap(DirectX::DescriptorHeap* pDescriptorHeap) noexcept {
    m_pDescriptorHeap = pDescriptorHeap;
}

void MaterialDeviceData::SetCommonStates(DirectX::CommonStates* pCommonStates) noexcept {
    m_pCommonStates = pCommonStates;
}

void MaterialDeviceData::SetRtState(DirectX::RenderTargetState* pRtState) noexcept {
    m_pRtState = pRtState;
}

