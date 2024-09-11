#include "DeviceDataBatch.h"

DeviceDataBatch::DeviceDataBatch(DeviceResources& deviceResources, std::uint8_t descriptorHeapSize, const bool& msaaEnabled) 
    noexcept : m_deviceResources(deviceResources),
    m_msaaEnabled(msaaEnabled),
    m_descriptorHeapSize(descriptorHeapSize),
    m_nextDescriptorHeapIndex(0)
{
    m_deviceResources.RegisterDeviceObserver(this);
}

DeviceDataBatch::~DeviceDataBatch() noexcept {
}

DeviceDataBatch::DeviceDataBatch(DeviceDataBatch& other) 
    noexcept : m_deviceResources(other.m_deviceResources),
    m_msaaEnabled(other.m_msaaEnabled),
    m_descriptorHeapSize(other.m_descriptorHeapSize)
{}

void DeviceDataBatch::OnDeviceLost() {
    m_pDescriptorHeap.reset();
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

void DeviceDataBatch::OnDeviceRestored() {

}

void DeviceDataBatch::Add(const AssetBatch& batch) {
    for (auto& modelPair : batch.GetModels()) {
        Add(modelPair.second);
    }
    for (auto& spritePair : batch.GetSprites()) {
        Add(spritePair.second);
    }
    for (auto& textPair : batch.GetTexts()) {
        Add(textPair.second);
    }
}

void DeviceDataBatch::Add(std::shared_ptr<Material> pMaterial) {
    std::unique_ptr<DirectX::IEffect>& pIEffect = m_materialData[pMaterial];
    if (!pIEffect) {
        std::unique_ptr<TextureDeviceData>& pDiffData = m_textureData[pMaterial->GetDiffuseMapFilePath()];
        if (!pDiffData)
            pDiffData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
        std::unique_ptr<TextureDeviceData>& pNormData = m_textureData[pMaterial->GetNormalMapFilePath()];
        if (!pNormData) 
            pNormData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
        pIEffect = BuildIEffect(*pMaterial);
    }
}

void DeviceDataBatch::Add(std::shared_ptr<Model> pModel) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    std::unique_ptr<ModelDeviceData>& pModelData = m_modelData[pModel];

    if (!pModelData) {
        pModelData = std::make_unique<ModelDeviceData>(pDevice, pModel.get(), m_meshData);

        pModelData->GetEffects().reserve(pModel->GetNumMaterials());
        for (std::uint8_t i = 0; i < pModel->GetNumMaterials(); ++i) {
            std::shared_ptr<Material> pMaterial = pModel->GetMaterials()[i];
            Add(pMaterial); // Material should already be added by now but the redundancy doesn't effect performance much.
            pModelData->GetEffects().push_back(m_materialData.at(pMaterial).get());
        }
        pModelData->GetEffects().shrink_to_fit();
    }
}

void DeviceDataBatch::Add(std::shared_ptr<Sprite> pSprite) {
    m_spriteData[pSprite] = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceDataBatch::Add(std::shared_ptr<Text> pText) {
    m_textData[pText] = std::make_unique<TextDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceDataBatch::OnAdd(std::shared_ptr<Material>& pMaterial) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    std::unique_ptr<TextureDeviceData>& pDiffData = m_textureData[pMaterial->GetDiffuseMapFilePath()];
    if (!pDiffData) {
        pDiffData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
        CreateTextureResource(pMaterial->GetDiffuseMapFilePath(), pDiffData, resourceUploadBatch);
    }
    std::unique_ptr<TextureDeviceData>& pNormData = m_textureData[pMaterial->GetNormalMapFilePath()];
    if (!pNormData) {
        pNormData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
        CreateTextureResource(pMaterial->GetNormalMapFilePath(), pNormData, resourceUploadBatch);
    }
    std::unique_ptr<DirectX::IEffect>& pIEffect = m_materialData[pMaterial];
    if (!pIEffect) {
        pIEffect = BuildIEffect(*pMaterial);
        BindTexturesToEffect(*pMaterial, *pIEffect);
    }

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
    uploadResourceFinished.wait();
}

void DeviceDataBatch::OnAdd(std::shared_ptr<Model>& pModel) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    std::unique_ptr<ModelDeviceData>& pModelData = m_modelData[pModel];


    if (!pModelData) {
        DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
        resourceUploadBatch.Begin();

        pModelData = std::make_unique<ModelDeviceData>(pDevice, pModel.get(), m_meshData);
        pModelData->LoadStaticBuffers(pDevice, resourceUploadBatch);

        pModelData->GetEffects().reserve(pModel->GetNumMaterials());
        for (std::uint8_t i = 0; i < pModel->GetNumMaterials(); ++i) {
            std::shared_ptr<Material> pMaterial = pModel->GetMaterials()[i];
            OnAdd(pMaterial); // Material should already be added by now but the redundancy doesn't effect performance much.
            pModelData->GetEffects().push_back(m_materialData.at(pMaterial).get());
        }
        pModelData->GetEffects().shrink_to_fit();

        std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
        uploadResourceFinished.wait();
    }
}

void DeviceDataBatch::OnAdd(std::shared_ptr<Sprite>& pSprite) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    std::unique_ptr<TextureDeviceData>& pSpriteData = m_spriteData[pSprite]; 

    if (!pSpriteData) {
        DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
        resourceUploadBatch.Begin();

        pSpriteData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
        CreateSpriteResource(pSprite, pSpriteData, resourceUploadBatch);

        std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
        uploadResourceFinished.wait();
    }
}

void DeviceDataBatch::OnAdd(std::shared_ptr<Text>& pText) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    std::unique_ptr<TextDeviceData>& pTextData = m_textData[pText];

    if (!pTextData) {
        DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
        resourceUploadBatch.Begin();

        pTextData = std::make_unique<TextDeviceData>(m_nextDescriptorHeapIndex++);
        CreateTextResource(pText, pTextData, resourceUploadBatch);

        std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
        uploadResourceFinished.wait();
    }
}

void DeviceDataBatch::OnAdd(std::shared_ptr<Outline>& pOutline) {

}

void DeviceDataBatch::OnRemove(std::shared_ptr<Material>& pMaterial) {

}

void DeviceDataBatch::OnRemove(std::shared_ptr<Model>& pModel) {

}

void DeviceDataBatch::OnRemove(std::shared_ptr<Sprite>& pSprite) {

}

void DeviceDataBatch::OnRemove(std::shared_ptr<Text>& pText) {

}

void DeviceDataBatch::OnRemove(std::shared_ptr<Outline>& pOutline) {

}

void DeviceDataBatch::UpdateEffects(DirectX::XMMATRIX view, DirectX::XMMATRIX projection) {
    m_pOutlineEffect->SetView(view);
    m_pOutlineEffect->SetProjection(projection);

    for (MaterialPair& materialPair : m_materialData) {
        if (auto pIMatrices = dynamic_cast<DirectX::IEffectMatrices*>(materialPair.second.get())) {
            pIMatrices->SetView(view);
            pIMatrices->SetProjection(projection);
        }
        if (auto pNormal = dynamic_cast<DirectX::NormalMapEffect*>(materialPair.second.get())) {
            pNormal->SetDiffuseColor(DirectX::XMLoadFloat4(&materialPair.first->GetDiffuseColor()));
            pNormal->SetEmissiveColor(DirectX::XMLoadFloat4(&materialPair.first->GetEmissiveColor()));
            pNormal->SetSpecularColor(DirectX::XMLoadFloat4(&materialPair.first->GetSpecularColor()));
        }
    }
}

void DeviceDataBatch::CreateDeviceDependentResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    CreateDescriptorHeapResources();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    for (SpritePair& spritePair : m_spriteData) {
        CreateSpriteResource(spritePair.first, spritePair.second, resourceUploadBatch);
    }
    for (TextPair& textPair : m_textData) {
        CreateTextResource(textPair.first, textPair.second, resourceUploadBatch);
    }
    for (TexturePair& texturePair : m_textureData) {
        CreateTextureResource(texturePair.first, texturePair.second, resourceUploadBatch);
    }
    for (ModelPair& modelPair : m_modelData) {
        modelPair.second->LoadStaticBuffers(pDevice, resourceUploadBatch);
    }

    CreateRenderTargetDependentResources(resourceUploadBatch);

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
    uploadResourceFinished.wait();
}

void DeviceDataBatch::CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    for (MaterialPair& materialPair : m_materialData) {
        BindTexturesToEffect(*materialPair.first, *materialPair.second);
    }
    CreateOutlineBatchResources();
    CreateSpriteBatchResources(resourceUploadBatch);
}

void DeviceDataBatch::CreateWindowSizeDependentResources() {
    D3D12_VIEWPORT viewport = m_deviceResources.GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}

void DeviceDataBatch::CreateDescriptorHeapResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    m_pDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(
            pDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            m_descriptorHeapSize);
    m_pStates = std::make_unique<DirectX::CommonStates>(pDevice);
}

void DeviceDataBatch::CreateSpriteBatchResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    DirectX::SpriteBatchPipelineStateDescription pd(RenderTargetState());
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);
}

void DeviceDataBatch::CreateSpriteResource(const std::shared_ptr<Sprite>& pSprite, std::unique_ptr<TextureDeviceData>& pSpriteData, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    CreateTextureFromFile(pSprite->GetFilePath(), pSpriteData->GetTexture().ReleaseAndGetAddressOf(), resourceUploadBatch);
    DirectX::CreateShaderResourceView(pDevice, pSpriteData->GetTexture().Get(),
            m_pDescriptorHeap->GetCpuHandle(pSpriteData->GetHeapIndex()));
}

void DeviceDataBatch::CreateTextResource(const std::shared_ptr<Text>& pText, std::unique_ptr<TextDeviceData>& pTextData, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    if (!CompareFileExtension(pText->GetFilePath(), L".spritefont")) 
        throw std::invalid_argument("Unsupported file extension detected.");

    auto pFont = std::make_unique<DirectX::SpriteFont>(
            pDevice,
            resourceUploadBatch,
            pText->GetFilePath().c_str(),
            m_pDescriptorHeap->GetCpuHandle(pTextData->GetHeapIndex()),
            m_pDescriptorHeap->GetGpuHandle(pTextData->GetHeapIndex()));
    pTextData->SetSpriteFont(std::move(pFont));
}

void DeviceDataBatch::CreateTextureResource(const std::wstring& fileName, std::unique_ptr<TextureDeviceData>& pTextureData, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    CreateTextureFromFile(
            fileName,
            pTextureData->GetTexture().ReleaseAndGetAddressOf(), 
            resourceUploadBatch);
    DirectX::CreateShaderResourceView(
            pDevice, 
            pTextureData->GetTexture().Get(),
            m_pDescriptorHeap->GetCpuHandle(pTextureData->GetHeapIndex()));
}

void DeviceDataBatch::BindTexturesToEffect(Material& material, DirectX::IEffect& iEffect) {
    auto& effect = static_cast<DirectX::NormalMapEffect&>(iEffect);
    effect.SetTexture(
            m_pDescriptorHeap->GetGpuHandle(m_textureData.at(material.GetDiffuseMapFilePath())->GetHeapIndex()),
            SamplerDesc(material.GetFlags()));
    effect.SetNormalTexture(
            m_pDescriptorHeap->GetGpuHandle(m_textureData.at(material.GetNormalMapFilePath())->GetHeapIndex()));
}

void DeviceDataBatch::CreateOutlineBatchResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

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
            RenderTargetState(),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    m_pOutlineEffect = std::make_unique<DirectX::BasicEffect>(pDevice, DirectX::EffectFlags::VertexColor, pd);
}

std::unique_ptr<DirectX::IEffect> DeviceDataBatch::BuildIEffect(Material& material) const {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    std::uint32_t flags = material.GetFlags();
    std::unique_ptr<DirectX::NormalMapEffect> pEffect;

    D3D12_INPUT_LAYOUT_DESC inputLayout = InputLayoutDesc(flags);
    DirectX::EffectPipelineStateDescription pd(
            &inputLayout,
            BlendDesc(flags),
            DepthStencilDesc(flags),
            RasterizerDesc(flags),
            RenderTargetState());

    if (flags & RenderFlags::Effect::Instanced)
        pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Instancing, pd);
    else if (flags & RenderFlags::Effect::Skinned) 
        pEffect = std::make_unique<DirectX::SkinnedNormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting, pd);
    else
        pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting | DirectX::EffectFlags::Texture, pd);

    pEffect->SetColorAndAlpha(DirectX::XMLoadFloat4(&material.GetDiffuseColor()));
    pEffect->SetEmissiveColor(DirectX::XMLoadFloat4(&material.GetEmissiveColor()));
    pEffect->SetSpecularColor(DirectX::XMLoadFloat4(&material.GetSpecularColor()));

    pEffect->EnableDefaultLighting();

    return std::move(pEffect);
}

DirectX::RenderTargetState DeviceDataBatch::RenderTargetState() const noexcept {
    DirectX::RenderTargetState rtState(
            m_deviceResources.GetBackBufferFormat(), 
            m_deviceResources.GetDepthBufferFormat());
    if (m_msaaEnabled) {
        rtState.sampleDesc.Count = DeviceResources::MSAA_COUNT;
        rtState.sampleDesc.Quality = DeviceResources::MSAA_QUALITY;
    }
    return rtState;
}

D3D12_INPUT_LAYOUT_DESC DeviceDataBatch::InputLayoutDesc(std::uint32_t flags) const {
    if (flags & RenderFlags::Effect::Instanced)
        return VertexPositionNormalTexture::InputLayoutInstancing;
    if (flags & RenderFlags::Effect::Skinned)
        return VertexPositionNormalTextureSkinning::InputLayout;
    return VertexPositionNormalTexture::InputLayout;
}

D3D12_BLEND_DESC DeviceDataBatch::BlendDesc(std::uint32_t flags) const noexcept {
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

D3D12_DEPTH_STENCIL_DESC DeviceDataBatch::DepthStencilDesc(std::uint32_t flags) const noexcept {
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

D3D12_RASTERIZER_DESC DeviceDataBatch::RasterizerDesc(std::uint32_t flags) const noexcept {
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

D3D12_GPU_DESCRIPTOR_HANDLE DeviceDataBatch::SamplerDesc(std::uint32_t flags) const noexcept {
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

bool DeviceDataBatch::CompareFileExtension(std::wstring filePath, std::wstring valid) {
    std::wstring extension = filePath.substr(filePath.size() - valid.size());
    return extension == valid;
}

void DeviceDataBatch::CreateTextureFromFile(std::wstring filePath, ID3D12Resource** ppTexture, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    bool isDDS = CompareFileExtension(filePath, L".dds");
    if (isDDS) {
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(pDevice, resourceUploadBatch, filePath.c_str(), ppTexture));
    } else if (CompareFileExtension(filePath, L".png") || 
            CompareFileExtension(filePath, L".jpg") ||
            CompareFileExtension(filePath, L".bmp") ||
            CompareFileExtension(filePath, L".gif") || 
            CompareFileExtension(filePath, L".tiff")) {
        ThrowIfFailed(DirectX::CreateWICTextureFromFile(pDevice, resourceUploadBatch, filePath.c_str(), ppTexture));  
    } else {
        throw std::invalid_argument("Unsupported file extension detected.");
    }
}

bool DeviceDataBatch::HasMaterials() const noexcept {
    return m_materialData.size() > 0;
}

bool DeviceDataBatch::HasTextures() const noexcept {
    return GetNumDescriptors() > 0;
}

DirectX::DescriptorHeap* DeviceDataBatch::GetDescriptorHeap() const noexcept {
    return m_pDescriptorHeap.get();
}

DirectX::CommonStates* DeviceDataBatch::GetStates() const noexcept {
    return m_pStates.get();
}

DirectX::SpriteBatch* DeviceDataBatch::GetSpriteBatch() const noexcept {
    return m_pSpriteBatch.get();
}

DirectX::BasicEffect* DeviceDataBatch::GetOutlineEffect() const noexcept {
    return m_pOutlineEffect.get();
}

DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* DeviceDataBatch::GetOutlineBatch() const noexcept {
    return m_pOutlinePrimitiveBatch.get();
}

const std::unordered_map<std::shared_ptr<Model>, std::unique_ptr<ModelDeviceData>>& DeviceDataBatch::GetModelData() const noexcept {
    return m_modelData;
}

const std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>>& DeviceDataBatch::GetSpriteData() const noexcept {
    return m_spriteData;
}

const std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>>& DeviceDataBatch::GetTextData() const noexcept {
    return m_textData;
}

std::uint8_t DeviceDataBatch::GetNumDescriptors() const noexcept {
    return m_spriteData.size() + m_textData.size() + m_textureData.size() + 1; // +1 for ImGui font texture's.
}

