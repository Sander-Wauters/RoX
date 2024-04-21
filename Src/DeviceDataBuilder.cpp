#include "DeviceDataBuilder.h"

#include <unordered_set>

#include <DescriptorHeap.h>
#include <ResourceUploadBatch.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXHelpers.h>

#include "Exceptions/ThrowIfFailed.h"

DeviceDataBuilder::DeviceDataBuilder(Scene& scene, const DeviceResources& deviceResources) 
    noexcept : m_scene(scene), 
    m_deviceResources(deviceResources),
    m_nextDescriptorHeapIndex(0),
    m_pResourceDescriptors(nullptr),
    m_pStates(nullptr),
    m_pSpriteBatch(nullptr),
    m_pOutlineEffect(nullptr),
    m_pOutlinePrimitiveBatch(nullptr)
{
    m_scene.RegisterSceneObserver(this);

    for (auto& mesh : m_scene.GetMeshes()) {
        OnAdd(mesh.second);
    }
    for (auto& sprite : m_scene.GetSprites()) {
        OnAdd(sprite.second);
    }
    for (auto& text : m_scene.GetText()) {
        OnAdd(text.second);
    }
    for (auto& outline : m_scene.GetOutlines()) {
        OnAdd(outline.second);
    }
}

DeviceDataBuilder::~DeviceDataBuilder() noexcept {
}

void DeviceDataBuilder::OnDeviceLost() {
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

void DeviceDataBuilder::OnDeviceRestored() {
}

void DeviceDataBuilder::OnAdd(std::shared_ptr<Mesh> pMesh) {
    std::unique_ptr<MeshDeviceData>& pMeshData = m_meshData[pMesh];
    if (!pMeshData) {
        pMeshData = std::make_unique<MeshDeviceData>();

        pMeshData->meshParts.reserve(pMesh->GetMeshParts().size());
        for (auto& pMeshPart : pMesh->GetMeshParts()) {
            std::shared_ptr<MeshPartDeviceData>& pMeshPartData = m_meshPartData[pMeshPart];
            if (!pMeshPartData) {
                pMeshPartData = std::make_shared<MeshPartDeviceData>();

                pMeshPartData->submeshes.reserve(pMeshPart->GetSubmeshes().size());
                for (std::uint64_t i = 0; i < pMeshPart->GetSubmeshes().size(); ++i) {
                    pMeshPartData->submeshes.push_back(std::make_unique<SubmeshDeviceData>());
                }
            }
            pMeshData->meshParts.push_back(pMeshPartData);
        }
        pMeshData->meshParts.shrink_to_fit();

        pMeshData->effects.reserve(pMesh->GetMaterials().size());
        for (auto& pMaterial : pMesh->GetMaterials()) {
            std::unique_ptr<DirectX::IEffect>& pEffect = m_materialData[pMaterial];
            if (!pEffect) {
                pMeshData->effects.push_back(&pEffect);

                std::unique_ptr<TextureDeviceData>& pDiffData = m_textureData[pMaterial->GetDiffuseMapFilePath()];
                if (!pDiffData)
                    pDiffData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);

                std::unique_ptr<TextureDeviceData>& pNormData = m_textureData[pMaterial->GetNormalMapFilePath()];
                if (!pNormData) 
                    pNormData = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
            }
        }
        pMeshData->effects.shrink_to_fit();
    }
}

void DeviceDataBuilder::OnAdd(std::shared_ptr<Sprite> pSprite) {
    m_spriteData[pSprite] = std::make_unique<TextureDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceDataBuilder::OnAdd(std::shared_ptr<Text> pText) {
    m_textData[pText] = std::make_unique<TextDeviceData>(m_nextDescriptorHeapIndex++);
}

void DeviceDataBuilder::OnAdd(std::shared_ptr<Outline::Base> pOutline) {

}

void DeviceDataBuilder::OnRemove(std::shared_ptr<Mesh> pMesh) {

}

void DeviceDataBuilder::OnRemove(std::shared_ptr<Sprite> pSprite) {

}

void DeviceDataBuilder::OnRemove(std::shared_ptr<Text> pText) {

}

void DeviceDataBuilder::OnRemove(std::shared_ptr<Outline::Base> pOutline) {

}

void DeviceDataBuilder::Update() {
    // TODO: Client should be able to decide this.
    // Store the view and projection in the object that the client has access to.
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetProjection());

    m_pOutlineEffect->SetView(view);
    m_pOutlineEffect->SetProjection(projection);

    for (auto& pEffect : m_materialData) {
        auto* effect = static_cast<DirectX::NormalMapEffect*>(pEffect.second.get());

        effect->SetView(view);
        effect->SetProjection(projection);
    }
}

bool DeviceDataBuilder::HasMaterials() const noexcept {
    return m_materialData.size() > 0;
}

bool DeviceDataBuilder::HasTextures() const noexcept {
    return GetResourceDescriptorCount() > 0;
}

void DeviceDataBuilder::BuildDeviceDependentResources(bool msaaEnabled) {
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

void DeviceDataBuilder::BuildSprites(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (auto& sprite : m_spriteData) {
        CreateTextureFromFile(sprite.first->GetFilePath(), sprite.second->pTexture.ReleaseAndGetAddressOf(), resourceUploadBatch);

        DirectX::CreateShaderResourceView(pDevice, sprite.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(sprite.second->descriptorHeapIndex));
    }
}

void DeviceDataBuilder::BuildText(DirectX::ResourceUploadBatch& resourceUploadBatch) {
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

void DeviceDataBuilder::BuildTextures(DirectX::ResourceUploadBatch& resourceUploadBatch) {
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

void DeviceDataBuilder::BuildRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::RenderTargetState rtState(
            m_deviceResources.GetBackBufferFormat(), 
            m_deviceResources.GetDepthBufferFormat());
    if (msaaEnabled) {
        rtState.sampleDesc.Count = DeviceResources::MSAA_COUNT;
        rtState.sampleDesc.Quality = DeviceResources::MSAA_QUALITY;
    }

    BuildMeshes(rtState, resourceUploadBatch);
    BuildMaterials(rtState);
    BuildOutlines(rtState);

    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pDevice, resourceUploadBatch, pd);
}

void DeviceDataBuilder::BuildMeshes(DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    OutputDebugString("====================================================\n");
    for(auto& meshPart : m_meshPartData) {
        std::string debug = meshPart.first->GetName() + "\n";
        OutputDebugString(debug.c_str());
        for (std::uint64_t i = 0; i < meshPart.first->GetSubmeshes().size(); ++i) {
            debug = "   " + meshPart.first->GetSubmeshes()[i]->GetName() + "\n";
            OutputDebugString(debug.c_str());
            SubmeshDeviceData* pSubmesh = meshPart.second->submeshes[i].get();
            std::vector<DirectX::VertexPositionNormalTexture>* pVertices = meshPart.first->GetSubmeshes()[i]->GetVertices().get();
            std::vector<std::uint16_t>* pIndices = meshPart.first->GetSubmeshes()[i]->GetIndices().get();

            if (pVertices->size() >= USHRT_MAX)
                throw std::invalid_argument("Too many vertices for a 16-bit index buffer");
            if (pIndices->size() > UINT32_MAX)
                throw std::invalid_argument("Too many indices");

            std::uint64_t sizeInBytes = static_cast<std::uint32_t>(pVertices->size()) * sizeof(DirectX::VertexPositionNormalTexture);
            if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
                throw std::invalid_argument("VB too large for DirectX 12");

            pSubmesh->vertexBufferSize = sizeInBytes;      
            pSubmesh->vertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
            memcpy(pSubmesh->vertexBuffer.Memory(), pVertices->data(), sizeInBytes);

            sizeInBytes = static_cast<std::uint32_t>(pIndices->size()) * sizeof(std::uint16_t);
            if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
                throw std::invalid_argument("IB too large for DirectX 12");

            pSubmesh->indexBufferSize = sizeInBytes;
            pSubmesh->indexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
            memcpy(pSubmesh->indexBuffer.Memory(), pIndices->data(), sizeInBytes);
        }
    }

    for (auto& mesh : m_meshData) {
        mesh.second->LoadStaticBuffers(pDevice, resourceUploadBatch);
    }
}

void DeviceDataBuilder::BuildMaterials(DirectX::RenderTargetState& renderTargetState) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

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
            &layout,
            DirectX::CommonStates::Opaque,
            DirectX::CommonStates::DepthDefault,
            DirectX::CommonStates::CullCounterClockwise,
            renderTargetState);

    for (auto& effect : m_materialData) {
        auto pEffect = std::make_unique<DirectX::NormalMapEffect>(pDevice, DirectX::EffectFlags::Instancing, pd);
        pEffect->EnableDefaultLighting();
        pEffect->SetTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textureData.at(effect.first->GetDiffuseMapFilePath())->descriptorHeapIndex),
                m_pStates->AnisotropicWrap());
        pEffect->SetNormalTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textureData.at(effect.first->GetNormalMapFilePath())->descriptorHeapIndex));

        effect.second = std::move(pEffect);
    }

}

void DeviceDataBuilder::BuildOutlines(DirectX::RenderTargetState& renderTargetState) {
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

void DeviceDataBuilder::BuildWindowSizeDependentResources() {
    D3D12_VIEWPORT viewport = m_deviceResources.GetScreenViewport();
    m_pSpriteBatch->SetViewport(viewport);
}

bool DeviceDataBuilder::CompareFileExtension(std::wstring filePath, std::wstring valid) {
    std::wstring extension = filePath.substr(filePath.size() - valid.size());
    return extension == valid;
}

void DeviceDataBuilder::CreateTextureFromFile(std::wstring filePath, ID3D12Resource** pTexture, DirectX::ResourceUploadBatch& resourceUploadBatch){
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

Scene& DeviceDataBuilder::GetScene() const noexcept {
    return m_scene;
}

DirectX::DescriptorHeap* DeviceDataBuilder::GetDescriptorHeap() const noexcept {
    return m_pResourceDescriptors.get();
}

DirectX::CommonStates* DeviceDataBuilder::GetStates() const noexcept {
    return m_pStates.get();
}

DirectX::SpriteBatch* DeviceDataBuilder::GetSpriteBatch() const noexcept {
    return m_pSpriteBatch.get();
}

DirectX::BasicEffect* DeviceDataBuilder::GetOutlineEffect() const noexcept {
    return m_pOutlineEffect.get();
}

DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* DeviceDataBuilder::GetOutlineBatch() const noexcept {
    return m_pOutlinePrimitiveBatch.get();
}

const std::unordered_map<std::shared_ptr<Mesh>, std::unique_ptr<MeshDeviceData>>& DeviceDataBuilder::GetMeshData() const noexcept {
    return m_meshData;
}

const std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>>& DeviceDataBuilder::GetSpriteData() const noexcept {
    return m_spriteData;
}

const std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>>& DeviceDataBuilder::GetTextData() const noexcept {
    return m_textData;
}

size_t DeviceDataBuilder::GetResourceDescriptorCount() const noexcept {
    return m_spriteData.size() + m_textData.size() + m_textureData.size(); 
}


