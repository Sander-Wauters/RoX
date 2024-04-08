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

    for (const std::pair<const std::string, std::shared_ptr<Mesh>>& mesh : m_scene.GetMeshes()) {
        OnAdd(mesh.second.get());
    }
    for (const std::pair<const std::string, std::shared_ptr<Sprite>>& sprite : m_scene.GetSprites()) {
        OnAdd(sprite.second.get());
    }
    for (const std::pair<const std::string, std::shared_ptr<Text>>& text : m_scene.GetText()) {
        OnAdd(text.second.get());
    }
    for (const std::pair<const std::string, std::shared_ptr<Outline::Base>>& outline : m_scene.GetOutlines()) {
        OnAdd(outline.second.get());
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

    for (std::pair<Mesh* const, std::unique_ptr<DeviceData::Mesh>>& mesh : m_meshData) {
        mesh.second->pGeometricPrimitive.reset();
    }
    for (std::pair<const Material* const, std::unique_ptr<DeviceData::Material>>& material : m_materialData) {
        material.second->pEffect.reset();
    }
    for (std::pair<Sprite* const, std::unique_ptr<DeviceData::Texture>>& sprite : m_spriteData) {
        sprite.second->pTexture.Reset();
    }
    for (std::pair<Text* const, std::unique_ptr<DeviceData::Text>>& text : m_textData) {
        text.second->pSpriteFont.reset();
    }
    for (std::pair<std::wstring const, std::unique_ptr<DeviceData::Texture>>& texture : m_textureData) {
        texture.second->pTexture.Reset();
    }
}

void DeviceDataBuilder::OnDeviceRestored() {
}

void DeviceDataBuilder::OnAdd(Mesh* pMesh) {
    m_meshData[pMesh] = std::make_unique<DeviceData::Mesh>();
    m_materialData[&pMesh->GetMaterial()] = std::make_unique<DeviceData::Material>();

    std::unique_ptr<DeviceData::Texture>& pDiffData = m_textureData[pMesh->GetMaterial().GetDiffuseMapFilePath()];
    if (!pDiffData) {
        pDiffData = std::make_unique<DeviceData::Texture>();
        pDiffData->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
    }

    std::unique_ptr<DeviceData::Texture>& pNormData = m_textureData[pMesh->GetMaterial().GetNormalMapFilePath()];
    if (!pNormData) {
        pNormData = std::make_unique<DeviceData::Texture>();
        pNormData->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
    }
}

void DeviceDataBuilder::OnAdd(Sprite* pSprite) {
    std::unique_ptr<DeviceData::Texture> pSpriteData = std::make_unique<DeviceData::Texture>();    
    pSpriteData->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
    m_spriteData[pSprite] = std::move(pSpriteData);
}

void DeviceDataBuilder::OnAdd(Text* pText) {
    std::unique_ptr<DeviceData::Text> pTextData = std::make_unique<DeviceData::Text>();
    pTextData->DescriptorHeapIndex = m_nextDescriptorHeapIndex++;
    m_textData[pText] = std::move(pTextData);
}

void DeviceDataBuilder::OnAdd(Outline::Base* pOutline) {

}

void DeviceDataBuilder::OnRemove(Mesh* pMesh) {

}

void DeviceDataBuilder::OnRemove(Sprite* pSprite) {

}

void DeviceDataBuilder::OnRemove(Text* pText) {

}

void DeviceDataBuilder::OnRemove(Outline::Base* pOutline) {

}

void DeviceDataBuilder::Update() {
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_scene.GetCamera().GetProjection());

    m_pOutlineEffect->SetView(view);
    m_pOutlineEffect->SetProjection(projection);

    for (std::pair<const Material* const, std::unique_ptr<DeviceData::Material>>& material : m_materialData) {
        material.second->pEffect->SetView(view);
        material.second->pEffect->SetProjection(projection);
    }
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

    BuildSpriteData(resourceUploadBatch);
    BuildTextData(resourceUploadBatch);
    BuildTextureData(resourceUploadBatch);

    BuildRenderTargetDependentResources(resourceUploadBatch, msaaEnabled);

    std::future<void> uploadResourceFinished = resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
    uploadResourceFinished.wait();
}

void DeviceDataBuilder::BuildSpriteData(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (std::pair<Sprite* const, std::unique_ptr<DeviceData::Texture>>& sprite : m_spriteData) {
        CreateTextureFromFile(sprite.first->GetFilePath(), sprite.second->pTexture.ReleaseAndGetAddressOf(), resourceUploadBatch);

        DirectX::CreateShaderResourceView(pDevice, sprite.second->pTexture.Get(),
                m_pResourceDescriptors->GetCpuHandle(sprite.second->DescriptorHeapIndex));
    }
}

void DeviceDataBuilder::BuildTextData(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (std::pair<Text* const, std::unique_ptr<DeviceData::Text>>& text : m_textData) {
        if (!CompareFileExtension(text.first->GetFilePath(), L".spritefont")) 
            throw std::invalid_argument("Unsupported file extension detected.");
            
        std::unique_ptr<DirectX::SpriteFont> pFont = std::make_unique<DirectX::SpriteFont>(
                pDevice,
                resourceUploadBatch,
                text.first->GetFilePath().c_str(),
                m_pResourceDescriptors->GetCpuHandle(text.second->DescriptorHeapIndex),
                m_pResourceDescriptors->GetGpuHandle(text.second->DescriptorHeapIndex));
        text.second->pSpriteFont = std::move(pFont);
    }
}

void DeviceDataBuilder::BuildTextureData(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    for (std::pair<std::wstring const, std::unique_ptr<DeviceData::Texture>>& texture : m_textureData) {
            CreateTextureFromFile(
                    texture.first,
                    texture.second->pTexture.ReleaseAndGetAddressOf(), 
                    resourceUploadBatch);
            DirectX::CreateShaderResourceView(
                    pDevice, 
                    texture.second->pTexture.Get(),
                    m_pResourceDescriptors->GetCpuHandle(texture.second->DescriptorHeapIndex));
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

    for (std::pair<Mesh* const, std::unique_ptr<DeviceData::Mesh>>& mesh : m_meshData) {
        switch (mesh.first->GetType()) {
            case Mesh::Type::CUBE:
                DirectX::GeometricPrimitive::CreateCube(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, false);
                break;
            case Mesh::Type::BOX:
                DirectX::GeometricPrimitive::CreateBox(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), { 1.f, 1.f, 1.f}, false);
                break;
            case Mesh::Type::SPHERE:
                DirectX::GeometricPrimitive::CreateSphere(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, 16, false);
                break;
            case Mesh::Type::GEO_SPHERE:
                DirectX::GeometricPrimitive::CreateGeoSphere(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, 3, false);
                break;
            case Mesh::Type::CYLINDER:
                DirectX::GeometricPrimitive::CreateCylinder(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, 1, 32, false);
                break;
            case Mesh::Type::CONE:
                DirectX::GeometricPrimitive::CreateCone(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, 1, 32, false);
                break;
            case Mesh::Type::TORUS:
                DirectX::GeometricPrimitive::CreateTorus(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, 0.333f, 32, false);
                break;
            case Mesh::Type::TETRAHEDRON:
                DirectX::GeometricPrimitive::CreateTetrahedron(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, false);
                break;
            case Mesh::Type::OCTAHEDRON:
                DirectX::GeometricPrimitive::CreateOctahedron(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, false);
                break;
            case Mesh::Type::DODECAHEDRON:
                DirectX::GeometricPrimitive::CreateDodecahedron(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, false);
                break;
            case Mesh::Type::ICOSAHEDRON: 
                DirectX::GeometricPrimitive::CreateIcosahedron(
                        mesh.first->GetVertices(), mesh.first->GetIndices(), 1, false);
                break;
            case Mesh::Type::CUSTOM: // Is handles regardless in the line bellow.
                break;
        }

        mesh.second->pGeometricPrimitive = DirectX::GeometricPrimitive::CreateCustom(
                mesh.first->GetVertices(), mesh.first->GetIndices(), pDevice);
        
        // Updload to dedicated video memory for better performance.
        mesh.second->pGeometricPrimitive->LoadStaticBuffers(pDevice, resourceUploadBatch);
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

    for (std::pair<const Material* const, std::unique_ptr<DeviceData::Material>>& material : m_materialData) {
        material.second->pEffect = std::make_unique<DirectX::NormalMapEffect>(
                pDevice, DirectX::EffectFlags::Instancing, pd);
        material.second->pEffect->EnableDefaultLighting();
        material.second->pEffect->SetTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textureData.at(material.first->GetDiffuseMapFilePath())->DescriptorHeapIndex),
                m_pStates->AnisotropicWrap());
        material.second->pEffect->SetNormalTexture(
                m_pResourceDescriptors->GetGpuHandle(m_textureData.at(material.first->GetNormalMapFilePath())->DescriptorHeapIndex));
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

const std::unordered_map<Mesh*, std::unique_ptr<DeviceData::Mesh>>& DeviceDataBuilder::GetMeshData() const noexcept {
    return m_meshData;
}

const std::unordered_map<const Material*, std::unique_ptr<DeviceData::Material>>& DeviceDataBuilder::GetMaterialData() const noexcept {
    return m_materialData;
}

const std::unordered_map<Sprite*, std::unique_ptr<DeviceData::Texture>>& DeviceDataBuilder::GetSpriteData() const noexcept {
    return m_spriteData;
}

const std::unordered_map<Text*, std::unique_ptr<DeviceData::Text>>& DeviceDataBuilder::GetTextData() const noexcept {
    return m_textData;
}

DeviceData::Material* DeviceDataBuilder::GetMaterialData(const Material* pMaterial) const {
    return m_materialData.at(pMaterial).get();
}

DeviceData::Texture* DeviceDataBuilder::GetTextureData(std::wstring filePath) const {
    return m_textureData.at(filePath).get();
}

size_t DeviceDataBuilder::GetResourceDescriptorCount() const noexcept {
    return m_spriteData.size() + m_textData.size() + m_textureData.size(); 
}


