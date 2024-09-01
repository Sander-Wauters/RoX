#include "DeviceResourceData.h"

#include "RoX/VertexTypes.h"

DeviceResourceData::DeviceResourceData(Scene& scene, const DeviceResources& deviceResources) 
    noexcept : m_scene(scene), 
    m_deviceResources(deviceResources),
    m_nextDescriptorHeapIndex(1),
    m_pResourceDescriptors(nullptr),
    m_pStates(nullptr),
    m_pSpriteBatch(nullptr),
    m_pOutlineEffect(nullptr),
    m_pOutlinePrimitiveBatch(nullptr)
{
    for (auto& model : m_scene.GetModels()) {
        Add(model.second);
    }
    for (auto& sprite : m_scene.GetSprites()) {
        Add(sprite.second);
    }
    for (auto& text : m_scene.GetTexts()) {
        Add(text.second);
    }
}

DeviceResourceData::~DeviceResourceData() noexcept {
}

void DeviceResourceData::OnDeviceLost() {
    m_pResourceDescriptors.reset();
    m_pStates.reset();

    m_pSpriteBatch.reset();
    m_pOutlineEffect.reset();
    m_pOutlinePrimitiveBatch.reset();

    for (MaterialPair& materialPair : m_materialData) {
        materialPair.second.reset();
    }
    for (MeshPair& meshPair : m_meshData) {
        meshPair.second->OnDeviceLost();
    }
    for (TexturePair& texturePair : m_textureData) {
        texturePair.second->OnDeviceLost();
    }
    for (SpritePair& spritePair : m_spriteData) {
        spritePair.second->OnDeviceLost();
    }
    for (TextPair& textPair : m_textData) {
        textPair.second->OnDeviceLost();
    }
}

void DeviceResourceData::OnDeviceRestored() {
}

void DeviceResourceData::Add(std::shared_ptr<Model> pModel) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    std::unique_ptr<ModelDeviceData>& pModelData = m_modelData[pModel];

    if (!pModelData) {
        pModelData = std::make_unique<ModelDeviceData>(pDevice, pModel.get(), m_meshData);

        pModelData->GetEffects().reserve(pModel->GetNumMaterials());
        for (std::uint8_t i = 0; i < pModel->GetNumMaterials(); ++i) {
            std::shared_ptr<Material> pMaterial = pModel->GetMaterials()[i];

            std::unique_ptr<DirectX::IEffect>& pEffect = m_materialData[pMaterial];
            if (!pEffect) {
                pModelData->GetEffects().push_back(&pEffect);
            }

            std::unique_ptr<TextureDeviceData>& pDiffData = m_textureData[pMaterial->GetDiffuseMapFilePath()];
            if (!pDiffData)
                pDiffData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);

            std::unique_ptr<TextureDeviceData>& pNormData = m_textureData[pMaterial->GetNormalMapFilePath()];
            if (!pNormData) 
                pNormData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
        }
        pModelData->GetEffects().shrink_to_fit();
    }
}

void DeviceResourceData::Add(std::shared_ptr<Sprite> pSprite) {
    m_spriteData[pSprite] = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceResourceData::Add(std::shared_ptr<Text> pText) {
    m_textData[pText] = std::make_unique<TextDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceResourceData::Update() {
    // TODO: Client should be able to decide this.
    // Store the view and projection in the object that the client has access to.
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetProjection());

    m_pOutlineEffect->SetView(view);
    m_pOutlineEffect->SetProjection(projection);

    for (MaterialPair& materialPair : m_materialData) {
        if (auto pIMatrices = dynamic_cast<DirectX::IEffectMatrices*>(materialPair.second.get())) {
            pIMatrices->SetView(view);
            pIMatrices->SetProjection(projection);
        }
        if (auto pNormal = dynamic_cast<DirectX::NormalMapEffect*>(materialPair.second.get())) {
            pNormal->SetDiffuseColor(materialPair.first->GetDiffuseColor());
            pNormal->SetEmissiveColor(materialPair.first->GetEmissiveColor());
            pNormal->SetSpecularColor(materialPair.first->GetSpecularColor());
        }
    }
}

bool DeviceResourceData::HasMaterials() const noexcept {
    return m_materialData.size() > 0;
}

bool DeviceResourceData::HasTextures() const noexcept {
    return GetResourceDescriptorCount() > 0;
}

void DeviceResourceData::BuildDeviceDependentResources(bool msaaEnabled) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    m_pResourceDescriptors = std::make_unique<DirectX::DescriptorHeap>(
            pDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            GetResourceDescriptorCount());
    m_pStates = std::make_unique<DirectX::CommonStates>(pDevice);

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    for (SpritePair& spritePair : m_spriteData) {
        BuildSprite(pDevice, spritePair, resourceUploadBatch);
    }
    for (TextPair& textPair : m_textData) {
        BuildText(pDevice, textPair, resourceUploadBatch);
    }
    for (TexturePair& texturePair : m_textureData) {
        BuildTexture(pDevice, texturePair, resourceUploadBatch);
    }

    BuildRenderTargetDependentResources(resourceUploadBatch, msaaEnabled);

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
    uploadResourceFinished.wait();
}

void DeviceResourceData::BuildSprite(ID3D12Device* pDevice, SpritePair& spritePair, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    CreateTextureFromFile(spritePair.first->GetFilePath(), spritePair.second->GetTexture().ReleaseAndGetAddressOf(), resourceUploadBatch);
    DirectX::CreateShaderResourceView(pDevice, spritePair.second->GetTexture().Get(),
            m_pResourceDescriptors->GetCpuHandle(spritePair.second->GetHeapIndex()));
}

void DeviceResourceData::BuildText(ID3D12Device* pDevice, TextPair& textPair, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    if (!CompareFileExtension(textPair.first->GetFilePath(), L".spritefont")) 
        throw std::invalid_argument("Unsupported file extension detected.");

    auto pFont = std::make_unique<DirectX::SpriteFont>(
            pDevice,
            resourceUploadBatch,
            textPair.first->GetFilePath().c_str(),
            m_pResourceDescriptors->GetCpuHandle(textPair.second->GetHeapIndex()),
            m_pResourceDescriptors->GetGpuHandle(textPair.second->GetHeapIndex()));
    textPair.second->SetSpriteFont(std::move(pFont));

}

void DeviceResourceData::BuildTexture(ID3D12Device* pDevice, TexturePair& texturePair, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    CreateTextureFromFile(
            texturePair.first,
            texturePair.second->GetTexture().ReleaseAndGetAddressOf(), 
            resourceUploadBatch);
    DirectX::CreateShaderResourceView(
            pDevice, 
            texturePair.second->GetTexture().Get(),
            m_pResourceDescriptors->GetCpuHandle(texturePair.second->GetHeapIndex()));
}

void DeviceResourceData::BuildRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::RenderTargetState rtState(
            m_deviceResources.GetBackBufferFormat(), 
            m_deviceResources.GetDepthBufferFormat());

    if (msaaEnabled) {
        rtState.sampleDesc.Count = DeviceResources::MSAA_COUNT;
        rtState.sampleDesc.Quality = DeviceResources::MSAA_QUALITY;
    }

    for (ModelPair& modelPair : m_modelData) {
        BuildModel(pDevice, modelPair, rtState, resourceUploadBatch);
    }
    for (MaterialPair& materialPair : m_materialData) {
        BuildMaterial(pDevice, materialPair, rtState);
    }
    BuildOutlines(pDevice, rtState);

    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);
}

void DeviceResourceData::BuildModel(ID3D12Device* pDevice, ModelPair& modelPair, DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    modelPair.second->LoadStaticBuffers(pDevice, resourceUploadBatch);
}

void DeviceResourceData::BuildMaterial(ID3D12Device* pDevice, MaterialPair& materialPair, DirectX::RenderTargetState& renderTargetState) {
    std::uint32_t flags = materialPair.first->GetFlags();
    std::unique_ptr<DirectX::NormalMapEffect> pEffect;

    D3D12_INPUT_LAYOUT_DESC inputLayout = InputLayout(materialPair.first->GetFlags());
    DirectX::EffectPipelineStateDescription pd(
            &inputLayout,
            BlendDesc(flags),
            DepthStencilDesc(flags),
            RasterizerDesc(flags),
            renderTargetState);

    if (flags & RenderFlags::Effect::Instanced)
        pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Instancing, pd);
    else if (flags & RenderFlags::Effect::Skinned) 
        pEffect = std::make_unique<DirectX::SkinnedNormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting, pd);
    else
        pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting | DirectX::EffectFlags::Texture, pd);

    pEffect->SetColorAndAlpha(materialPair.first->GetDiffuseColor());
    pEffect->SetEmissiveColor(materialPair.first->GetEmissiveColor());
    pEffect->SetSpecularColor(materialPair.first->GetSpecularColor());

    pEffect->EnableDefaultLighting();

    pEffect->SetTexture(
            m_pResourceDescriptors->GetGpuHandle(m_textureData.at(materialPair.first->GetDiffuseMapFilePath())->GetHeapIndex()),
            m_pStates->AnisotropicWrap());
    pEffect->SetNormalTexture(
            m_pResourceDescriptors->GetGpuHandle(m_textureData.at(materialPair.first->GetNormalMapFilePath())->GetHeapIndex()));

    materialPair.second = std::move(pEffect);
}

void DeviceResourceData::BuildOutlines(ID3D12Device* pDevice, DirectX::RenderTargetState& renderTargetState) {
    m_pOutlinePrimitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(pDevice);

    CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE, FALSE,
            D3D12_DEFAULT_DEPTH_BIAS, D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
            D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, TRUE, FALSE,
            0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

    DirectX::EffectPipelineStateDescription pd(
            &DirectX::VertexPositionColor::InputLayout,
            DirectX::CommonStates::Opaque,
            DirectX::CommonStates::DepthDefault,
            rastDesc,
            renderTargetState,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    m_pOutlineEffect = std::make_unique<DirectX::BasicEffect>(pDevice, DirectX::EffectFlags::VertexColor, pd);
}

void DeviceResourceData::BuildWindowSizeDependentResources() {
    D3D12_VIEWPORT viewport = m_deviceResources.GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}

D3D12_INPUT_LAYOUT_DESC DeviceResourceData::InputLayout(std::uint32_t flags) const {
    if (flags & RenderFlags::Effect::Instanced)
        return VertexPositionNormalTexture::InputLayoutInstancing;
    if (flags & RenderFlags::Effect::Skinned)
        return VertexPositionNormalTextureSkinning::InputLayout;
    
    return VertexPositionNormalTexture::InputLayout;
}

D3D12_BLEND_DESC DeviceResourceData::BlendDesc(std::uint32_t flags) const noexcept {
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

D3D12_DEPTH_STENCIL_DESC DeviceResourceData::DepthStencilDesc(std::uint32_t flags) const noexcept {
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

D3D12_RASTERIZER_DESC DeviceResourceData::RasterizerDesc(std::uint32_t flags) const noexcept {
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

D3D12_GPU_DESCRIPTOR_HANDLE DeviceResourceData::SemplerDesc(std::uint32_t flags) const noexcept {
    if (flags & RenderFlags::SamplerState::PointWrap)
        return m_pStates->PointWrap();
    if (flags & RenderFlags::SamplerState::PointClamp)
        return m_pStates->PointClamp();
    if (flags & RenderFlags::SamplerState::LinearWrap)
        return m_pStates->LinearWrap();
    if (flags & RenderFlags::SamplerState::LinearClamp)
        return m_pStates->LinearClamp();
    if (flags & RenderFlags::SamplerState::AnisotropicWrap)
        return m_pStates->AnisotropicWrap();
    if (flags & RenderFlags::SamplerState::AnisotropicClamp)
        return m_pStates->AnisotropicClamp();

    return m_pStates->AnisotropicWrap();
}

bool DeviceResourceData::CompareFileExtension(std::wstring filePath, std::wstring valid) {
    std::wstring extension = filePath.substr(filePath.size() - valid.size());
    return extension == valid;
}

void DeviceResourceData::CreateTextureFromFile(std::wstring filePath, ID3D12Resource** pTexture, DirectX::ResourceUploadBatch& resourceUploadBatch){
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    bool isDDS = CompareFileExtension(filePath, L".dds");
    if (isDDS) {
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(pDevice, resourceUploadBatch, filePath.c_str(), pTexture));
    } else if (CompareFileExtension(filePath, L".png") || 
            CompareFileExtension(filePath, L".jpg") ||
            CompareFileExtension(filePath, L".bmp") ||
            CompareFileExtension(filePath, L".gif") || 
            CompareFileExtension(filePath, L".tiff")) {
        ThrowIfFailed(DirectX::CreateWICTextureFromFile(pDevice, resourceUploadBatch, filePath.c_str(), pTexture));  
    } else {
        throw std::invalid_argument("Unsupported file extension detected.");
    }
}

Scene& DeviceResourceData::GetScene() const noexcept {
    return m_scene;
}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceResourceData::GetImGuiCpuDescHandle() const {
    return m_pResourceDescriptors->GetCpuHandle(c_ImGuiDescriptorIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE DeviceResourceData::GetImGuiGpuDescHandle() const {
    return m_pResourceDescriptors->GetGpuHandle(c_ImGuiDescriptorIndex);
}

DirectX::DescriptorHeap* DeviceResourceData::GetDescriptorHeap() const noexcept {
    return m_pResourceDescriptors.get();
}

DirectX::CommonStates* DeviceResourceData::GetStates() const noexcept {
    return m_pStates.get();
}

DirectX::SpriteBatch* DeviceResourceData::GetSpriteBatch() const noexcept {
    return m_pSpriteBatch.get();
}

DirectX::BasicEffect* DeviceResourceData::GetOutlineEffect() const noexcept {
    return m_pOutlineEffect.get();
}

DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* DeviceResourceData::GetOutlineBatch() const noexcept {
    return m_pOutlinePrimitiveBatch.get();
}

const std::unordered_map<std::shared_ptr<Model>, std::unique_ptr<ModelDeviceData>>& DeviceResourceData::GetModelData() const noexcept {
    return m_modelData;
}

const std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>>& DeviceResourceData::GetSpriteData() const noexcept {
    return m_spriteData;
}

const std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>>& DeviceResourceData::GetTextData() const noexcept {
    return m_textData;
}

size_t DeviceResourceData::GetResourceDescriptorCount() const noexcept {
    return m_spriteData.size() + m_textData.size() + m_textureData.size() + 1; // +1 for ImGui font texture's.
}


