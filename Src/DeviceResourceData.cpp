#include "DeviceResourceData.h"

#include <unordered_set>

#include <DescriptorHeap.h>
#include <ResourceUploadBatch.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXHelpers.h>

#include "Exceptions/ThrowIfFailed.h"

DeviceResourceData::DeviceResourceData(Scene& scene, const DeviceResources& deviceResources) 
    noexcept : m_scene(scene), 
    m_deviceResources(deviceResources),
    m_nextDescriptorHeapIndex(0),
    m_pResourceDescriptors(nullptr),
    m_pStates(nullptr),
    m_pSpriteBatch(nullptr),
    m_pOutlineEffect(nullptr),
    m_pOutlinePrimitiveBatch(nullptr)
{
    for (auto& model : m_scene.GetModels()) {
        InitDataFromScene(model.second);
    }
    for (auto& sprite : m_scene.GetSprites()) {
        InitDataFromScene(sprite.second);
    }
    for (auto& text : m_scene.GetText()) {
        InitDataFromScene(text.second);
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

    for (auto& effect : m_materialData) {
        effect.second.reset();
    }
    for (auto& texture : m_textureData) {
        texture.second->pTexture.Reset();
    }
    for (auto& sprite : m_spriteData) {
        sprite.second->pTexture.Reset();
    }
    for (auto& text : m_textData) {
        text.second->pSpriteFont.reset();
    }
}

void DeviceResourceData::OnDeviceRestored() {
}

void DeviceResourceData::InitDataFromScene(std::shared_ptr<Model> pModel) {
    std::unique_ptr<ModelDeviceData>& pModelData = m_modelData[pModel];
    if (!pModelData) {
        pModelData = std::make_unique<ModelDeviceData>();

        pModelData->meshes.reserve(pModel->GetMeshes().size());
        for (auto& pMesh : pModel->GetMeshes()) {
            std::shared_ptr<MeshDeviceData>& pMeshData = m_meshData[pMesh];
            if (!pMeshData) {
                pMeshData = std::make_shared<MeshDeviceData>();

                pMeshData->submeshes.reserve(pMesh->GetSubmeshes().size());
                for (std::uint64_t i = 0; i < pMesh->GetSubmeshes().size(); ++i) {
                    pMeshData->submeshes.push_back(std::make_unique<SubmeshDeviceData>());
                }
            }
            pModelData->meshes.push_back(pMeshData);
        }
        pModelData->meshes.shrink_to_fit();

        pModelData->effects.reserve(pModel->GetMaterials().size());
        for (auto& pMaterial : pModel->GetMaterials()) {
            std::unique_ptr<DirectX::IEffect>& pEffect = m_materialData[pMaterial];
            if (!pEffect) {
                pModelData->effects.push_back(&pEffect);

                std::unique_ptr<TextureDeviceData>& pDiffData = m_textureData[pMaterial->GetDiffuseMapFilePath()];
                if (!pDiffData)
                    pDiffData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);

                std::unique_ptr<TextureDeviceData>& pNormData = m_textureData[pMaterial->GetNormalMapFilePath()];
                if (!pNormData) 
                    pNormData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
            }
        }
        pModelData->effects.shrink_to_fit();
    }
}

void DeviceResourceData::InitDataFromScene(std::shared_ptr<Sprite> pSprite) {
    m_spriteData[pSprite] = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceResourceData::InitDataFromScene(std::shared_ptr<Text> pText) {
    m_textData[pText] = std::make_unique<TextDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceResourceData::Update() {
    // TODO: Client should be able to decide this.
    // Store the view and projection in the object that the client has access to.
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetProjection());

    m_pOutlineEffect->SetView(view);
    m_pOutlineEffect->SetProjection(projection);

    for (auto& pMaterial : m_materialData) {
        if (auto pEffect = dynamic_cast<DirectX::IEffectMatrices*>(pMaterial.second.get())) {
            pEffect->SetView(view);
            pEffect->SetProjection(projection);
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

    BuildSprites(resourceUploadBatch);
    BuildText(resourceUploadBatch);
    BuildTextures(resourceUploadBatch);

    BuildRenderTargetDependentResources(resourceUploadBatch, msaaEnabled);

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
    uploadResourceFinished.wait();
}

void DeviceResourceData::BuildSprites(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (auto& sprite : m_spriteData) {
        CreateTextureFromFile(sprite.first->GetFilePath(), sprite.second->pTexture.ReleaseAndGetAddressOf(), resourceUploadBatch);

        DirectX::CreateShaderResourceView(pDevice, sprite.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(sprite.second->descriptorHeapIndex));
    }
}

void DeviceResourceData::BuildText(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (auto& text : m_textData) {
        if (!CompareFileExtension(text.first->GetFilePath(), L".spritefont")) 
            throw std::invalid_argument("Unsupported file extension detected.");
            
        auto pFont = std::make_unique<DirectX::SpriteFont>(
                pDevice,
                resourceUploadBatch,
                text.first->GetFilePath().c_str(),
                m_pResourceDescriptors->GetCpuHandle(text.second->descriptorHeapIndex),
                m_pResourceDescriptors->GetGpuHandle(text.second->descriptorHeapIndex));
        text.second->pSpriteFont = std::move(pFont);
    }
}

void DeviceResourceData::BuildTextures(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (auto& texture : m_textureData) {
            CreateTextureFromFile(
                    texture.first,
                    texture.second->pTexture.ReleaseAndGetAddressOf(), 
                    resourceUploadBatch);
            DirectX::CreateShaderResourceView(
                    pDevice, 
                    texture.second->pTexture.Get(),
                    m_pResourceDescriptors->GetCpuHandle(texture.second->descriptorHeapIndex));
    }
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

    BuildModels(rtState, resourceUploadBatch);
    BuildMaterials(rtState);
    BuildOutlines(rtState);

    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);
}

void DeviceResourceData::BuildModels(DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    for(auto& mesh : m_meshData) {
        for (std::uint64_t submeshIndex = 0; submeshIndex < mesh.first->GetNumSubmeshes(); ++submeshIndex) {
            Submesh* pSubmesh = mesh.first->GetSubmeshes()[submeshIndex].get();
            SubmeshDeviceData* pSubmeshDeviceData = mesh.second->submeshes[submeshIndex].get();

            if (pSubmesh->GetNumVertices() >= USHRT_MAX)
                throw std::invalid_argument("Too many vertices for a 16-bit index buffer");
            if (pSubmesh->GetNumIndices() > UINT32_MAX)
                throw std::invalid_argument("Too many indices");

            std::uint32_t sizeInBytes = static_cast<std::uint32_t>(pSubmesh->GetNumVertices()) * sizeof(DirectX::VertexPositionNormalTexture);
            if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
                throw std::invalid_argument("VB too large for DirectX 12");

            pSubmeshDeviceData->vertexBufferSize = sizeInBytes;      
            pSubmeshDeviceData->vertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
            memcpy(pSubmeshDeviceData->vertexBuffer.Memory(), pSubmesh->GetVertices()->data(), sizeInBytes);

            sizeInBytes = static_cast<std::uint32_t>(pSubmesh->GetNumIndices()) * sizeof(std::uint16_t);
            if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
                throw std::invalid_argument("IB too large for DirectX 12");

            pSubmeshDeviceData->indexBufferSize = sizeInBytes;
            pSubmeshDeviceData->indexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
            memcpy(pSubmeshDeviceData->indexBuffer.Memory(), pSubmesh->GetIndices()->data(), sizeInBytes);
        }
    }

    for (auto& model : m_modelData) {
        model.second->LoadStaticBuffers(pDevice, resourceUploadBatch);
    }
}

void DeviceResourceData::BuildMaterials(DirectX::RenderTargetState& renderTargetState) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (auto& material : m_materialData) {
        std::uint32_t flags = material.first->GetFlags();
        std::unique_ptr<DirectX::NormalMapEffect> pEffect;

        const D3D12_INPUT_LAYOUT_DESC inputLayout = InputLayout(flags);
        DirectX::EffectPipelineStateDescription pd(
                &inputLayout,
                BlendDesc(flags),
                DepthStencilDesc(flags),
                RasterizerDesc(flags),
                renderTargetState);

        if (flags & RenderFlags::Effect::Instancing)
            pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Instancing, pd);
        else
            pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Lighting | DirectX::EffectFlags::Texture, pd);

        pEffect->SetColorAndAlpha(material.first->GetDiffuseColor());
        pEffect->SetEmissiveColor(material.first->GetEmissiveColor());
        pEffect->SetSpecularColor(material.first->GetSpecularColor());

        pEffect->EnableDefaultLighting();

        pEffect->SetTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textureData.at(material.first->GetDiffuseMapFilePath())->descriptorHeapIndex),
                m_pStates->AnisotropicWrap());
        pEffect->SetNormalTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textureData.at(material.first->GetNormalMapFilePath())->descriptorHeapIndex));

        material.second = std::move(pEffect);
    }

}

void DeviceResourceData::BuildOutlines(DirectX::RenderTargetState& renderTargetState) {
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
            renderTargetState,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);

    m_pOutlineEffect = std::make_unique<DirectX::BasicEffect>(pDevice, DirectX::EffectFlags::VertexColor, pd);
}

void DeviceResourceData::BuildWindowSizeDependentResources() {
    D3D12_VIEWPORT viewport = m_deviceResources.GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}

D3D12_INPUT_LAYOUT_DESC DeviceResourceData::InputLayout(std::uint32_t flags) const noexcept {
    if (flags & RenderFlags::Effect::Instancing)
        return { InstancedInputElements, static_cast<UINT>(std::size(InstancedInputElements)) };
    return DirectX::VertexPositionNormalTexture::InputLayout;
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
    return m_spriteData.size() + m_textData.size() + m_textureData.size(); 
}


