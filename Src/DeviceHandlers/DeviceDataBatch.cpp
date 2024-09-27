#include "DeviceDataBatch.h"

DeviceDataBatch::DeviceDataBatch(
        DeviceResources& deviceResources, 
        DirectX::CommonStates& commonStates,
        DirectX::RenderTargetState& rtState,
        std::uint8_t descriptorHeapSize) 
    noexcept : m_deviceResources(deviceResources),
    m_commonStates(commonStates),
    m_rtState(rtState),
    m_descriptorHeapSize(descriptorHeapSize),
    m_nextDescriptorHeapIndex(0)
{
    m_deviceResources.Attach(this);
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

DeviceDataBatch::~DeviceDataBatch() noexcept {
    m_deviceResources.Detach(this);

    for (MeshPair& meshPair : m_meshData) {
        meshPair.first->Detach(meshPair.second.get());
    }
    for (ModelPair& modelPair : m_modelData) {
        modelPair.first->Detach(modelPair.second.get());
    }
}

DeviceDataBatch::DeviceDataBatch(DeviceDataBatch& other) 
    noexcept : m_deviceResources(other.m_deviceResources),
    m_commonStates(other.m_commonStates),
    m_rtState(other.m_rtState),
    m_descriptorHeapSize(other.m_descriptorHeapSize)
{}

void DeviceDataBatch::OnDeviceLost() {
    m_pDescriptorHeap.reset();
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
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void DeviceDataBatch::OnAdd(const std::shared_ptr<Material>& pMaterial) {
    std::shared_ptr<TextureDeviceData>& pDiffData = m_textureData[pMaterial->GetDiffuseMapFilePath()];
    if (!pDiffData)
        pDiffData = std::make_unique<TextureDeviceData>(m_deviceResources, *m_pDescriptorHeap, NextHeapIndex(), pMaterial->GetDiffuseMapFilePath());

    std::shared_ptr<TextureDeviceData>& pNormData = m_textureData[pMaterial->GetNormalMapFilePath()];
    if (!pNormData)
        pNormData = std::make_unique<TextureDeviceData>(m_deviceResources, *m_pDescriptorHeap, NextHeapIndex(), pMaterial->GetNormalMapFilePath());

    std::shared_ptr<MaterialDeviceData>& pMaterialData = m_materialData[pMaterial];
    if (!pMaterialData)
        pMaterialData = std::make_unique<MaterialDeviceData>(
                m_deviceResources,
                *m_pDescriptorHeap,
                m_commonStates,
                m_rtState,
                pDiffData,
                pNormData,
                *pMaterial);
}

void DeviceDataBatch::OnAdd(const std::shared_ptr<Model>& pModel) {
    std::unique_ptr<ModelDeviceData>& pModelData = m_modelData[pModel];
    if (!pModelData)
        pModelData = std::make_unique<ModelDeviceData>(*this, *pModel);
}

void DeviceDataBatch::OnAdd(const std::shared_ptr<Sprite>& pSprite) {
    std::unique_ptr<TextureDeviceData>& pSpriteData = m_spriteData[pSprite]; 
    if (!pSpriteData)
        pSpriteData = std::make_unique<TextureDeviceData>(m_deviceResources, *m_pDescriptorHeap, NextHeapIndex(), pSprite->GetFilePath());
}

void DeviceDataBatch::OnAdd(const std::shared_ptr<Text>& pText) {
    std::unique_ptr<TextDeviceData>& pTextData = m_textData[pText];
    if (!pTextData)
        pTextData = std::make_unique<TextDeviceData>(
                m_deviceResources,
                *m_pDescriptorHeap,
                NextHeapIndex(),
                pText->GetFilePath());
}

void DeviceDataBatch::OnAdd(const std::shared_ptr<Outline>& pOutline) {

}

void DeviceDataBatch::OnRemove(const std::shared_ptr<Material>& pMaterial) {
    m_deviceResources.WaitForGpu();

    m_materialData.erase(pMaterial);

    auto& pDiffData = m_textureData.at(pMaterial->GetDiffuseMapFilePath());
    if (pDiffData.unique()) {
        m_openDescriptorHeapIndices.push(pDiffData->GetHeapIndex());
        m_textureData.erase(pMaterial->GetDiffuseMapFilePath());
    }

    if (pMaterial->GetDiffuseMapFilePath() == pMaterial->GetNormalMapFilePath())
        return;

    auto& pNormData = m_textureData.at(pMaterial->GetNormalMapFilePath());
    if (pNormData.unique()) {
        m_openDescriptorHeapIndices.push(pNormData->GetHeapIndex());
        m_textureData.erase(pMaterial->GetNormalMapFilePath());
    }
}

void DeviceDataBatch::OnRemove(const std::shared_ptr<Model>& pModel) {
    m_deviceResources.WaitForGpu();
    pModel->Detach(m_modelData.at(pModel).get());
    m_modelData.erase(pModel);

    for (auto it = m_meshData.begin(); it != m_meshData.end();) {
        if (it->second.unique())
            it = m_meshData.erase(it);
        else
            ++it;
    }
}

void DeviceDataBatch::OnRemove(const std::shared_ptr<Sprite>& pSprite) {
    m_deviceResources.WaitForGpu();
    m_openDescriptorHeapIndices.push(m_spriteData.at(pSprite)->GetHeapIndex());
    m_spriteData.erase(pSprite); 
}

void DeviceDataBatch::OnRemove(const std::shared_ptr<Text>& pText) {
    m_deviceResources.WaitForGpu();
    m_openDescriptorHeapIndices.push(m_textData.at(pText)->GetHeapIndex());
    m_textData.erase(pText);
}

void DeviceDataBatch::OnRemove(const std::shared_ptr<Outline>& pOutline) {

}

std::shared_ptr<MaterialDeviceData> DeviceDataBatch::GetMaterialDeviceData(const std::shared_ptr<Material>& pMaterial) {
    OnAdd(pMaterial);
    return m_materialData.at(pMaterial);
}

std::shared_ptr<MeshDeviceData> DeviceDataBatch::GetMeshDeviceData(const std::shared_ptr<IMesh>& pIMesh) {
    auto& pMeshDeviceData = m_meshData[pIMesh]; 
    if (!pMeshDeviceData)
        pMeshDeviceData = std::make_shared<MeshDeviceData>(m_deviceResources, *pIMesh);
    return pMeshDeviceData;
}

void DeviceDataBatch::SignalMeshRemoved() {
    m_deviceResources.WaitForGpu();

    for (auto it = m_meshData.begin(); it != m_meshData.end();) {
        if (it->second.unique()) {
            it->first->Detach(it->second.get());
            it = m_meshData.erase(it);
        } else
            ++it;
    }
}

void DeviceDataBatch::Add(const AssetBatch& batch) {
    for (auto& materialPair : batch.GetMaterials()) {
        OnAdd(materialPair.second);
    }
    for (auto& modelPair : batch.GetModels()) {
        OnAdd(modelPair.second);
    }
    for (auto& spritePair : batch.GetSprites()) {
        OnAdd(spritePair.second);
    }
    for (auto& textPair : batch.GetTexts()) {
        OnAdd(textPair.second);
    }
}

void DeviceDataBatch::Update(DirectX::XMMATRIX view, DirectX::XMMATRIX projection) {
    m_pOutlineEffect->SetView(view);
    m_pOutlineEffect->SetProjection(projection);

    for (MaterialPair& materialPair : m_materialData) {
        materialPair.second->UpdateIEffect(view, projection, *materialPair.first);
    }
}

void DeviceDataBatch::CreateDeviceDependentResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    CreateDescriptorHeapResources();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    for (ModelPair& modelPair : m_modelData) {
        //modelPair.second->LoadStaticBuffers(pDevice, resourceUploadBatch);
    }

    CreateRenderTargetDependentResources(resourceUploadBatch);

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
    uploadResourceFinished.wait();
}

void DeviceDataBatch::CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    for (MaterialPair& materialPair : m_materialData) {
        materialPair.second->CreateRenderTargetDependentResources();
    }
    CreateOutlineBatchResources();
    CreateSpriteBatchResources(resourceUploadBatch);
}

void DeviceDataBatch::CreateWindowSizeDependentResources() {
    D3D12_VIEWPORT viewport = m_deviceResources.GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}

std::uint8_t DeviceDataBatch::NextHeapIndex() noexcept {
    std::uint8_t nextIndex;
    if (m_openDescriptorHeapIndices.size() > 0) {
        nextIndex = m_openDescriptorHeapIndices.front();
        m_openDescriptorHeapIndices.pop();
    } else 
        nextIndex = m_nextDescriptorHeapIndex++;
    
    return nextIndex;
}

void DeviceDataBatch::CreateDescriptorHeapResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    m_pDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(
            pDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            m_descriptorHeapSize);
}

void DeviceDataBatch::CreateSpriteBatchResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    DirectX::SpriteBatchPipelineStateDescription pd(m_rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);
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
            m_rtState,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    m_pOutlineEffect = std::make_unique<DirectX::BasicEffect>(pDevice, DirectX::EffectFlags::VertexColor, pd);
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
    return m_spriteData.size() + m_textData.size() + m_textureData.size();
}

