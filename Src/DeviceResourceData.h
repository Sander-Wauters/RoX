#pragma once

#include <unordered_map>

#include <Effects.h>
#include <PrimitiveBatch.h>
#include <CommonStates.h>
#include <VertexTypes.h>

#include "Util/pch.h"

#include "RoX/Scene.h"

#include "IDeviceObserver.h"
#include "DeviceResources.h"
#include "ModelDeviceData.h"
#include "MeshDeviceData.h"
#include "TextureDeviceData.h"
#include "TextDeviceData.h"

using MaterialPair = std::pair<const std::shared_ptr<Material>, std::unique_ptr<DirectX::IEffect>>;
using ModelPair    = std::pair<const std::shared_ptr<Model>,    std::unique_ptr<ModelDeviceData>>;
using MeshPair     = std::pair<const std::shared_ptr<IMesh>,    std::unique_ptr<MeshDeviceData>>; 
using TexturePair  = std::pair<const std::wstring,              std::unique_ptr<TextureDeviceData>>;
using SpritePair   = std::pair<const std::shared_ptr<Sprite>,   std::unique_ptr<TextureDeviceData>>;
using TextPair     = std::pair<const std::shared_ptr<Text>,     std::unique_ptr<TextDeviceData>>;

class DeviceResourceData : public IDeviceObserver {
    private:
        static constexpr std::uint32_t c_ImGuiDescriptorIndex = 0;

    public:
        DeviceResourceData(Scene& scene, const DeviceResources& deviceResources) noexcept;
        ~DeviceResourceData() noexcept;

        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);

        void Update();

        bool HasMaterials() const noexcept;
        bool HasTextures() const noexcept;

        void BuildDeviceDependentResources(bool msaaEnabled);
        void BuildSprite(ID3D12Device* pDevice, SpritePair& spritePair, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildText(ID3D12Device* pDevice, TextPair& textPair, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTexture(ID3D12Device* pDevice, TexturePair& texturePair, DirectX::ResourceUploadBatch& resourceUploadBatch);

        void BuildRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled);
        void BuildModel(ID3D12Device* pDevice, ModelPair& modelPair, DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildMaterial(ID3D12Device* pDevice, MaterialPair& materialPair, DirectX::RenderTargetState& renderTargetState);
        void BuildOutlines(ID3D12Device* pDevice, DirectX::RenderTargetState& renderTargetState);

        void BuildWindowSizeDependentResources();
    
    private:
        D3D12_INPUT_LAYOUT_DESC InputLayout(std::uint32_t flags) const;
        D3D12_BLEND_DESC BlendDesc(std::uint32_t flags) const noexcept;
        D3D12_DEPTH_STENCIL_DESC DepthStencilDesc(std::uint32_t flags) const noexcept;
        D3D12_RASTERIZER_DESC RasterizerDesc(std::uint32_t flags) const noexcept;
        D3D12_GPU_DESCRIPTOR_HANDLE SemplerDesc(std::uint32_t flags) const noexcept;

        bool CompareFileExtension(std::wstring filePath, std::wstring valid);
        void CreateTextureFromFile(std::wstring filePath, ID3D12Resource** pTexture, DirectX::ResourceUploadBatch& resourceUploadBatch);

    public:
        Scene& GetScene() const noexcept;

        D3D12_CPU_DESCRIPTOR_HANDLE GetImGuiCpuDescHandle() const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetImGuiGpuDescHandle() const;

        DirectX::DescriptorHeap* GetDescriptorHeap() const noexcept;
        DirectX::CommonStates* GetStates() const noexcept;
        DirectX::SpriteBatch* GetSpriteBatch() const noexcept;

        DirectX::BasicEffect* GetOutlineEffect() const noexcept;
        DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* GetOutlineBatch() const noexcept;

        const std::unordered_map<std::shared_ptr<Model>, std::unique_ptr<ModelDeviceData>>& GetModelData() const noexcept;
        const std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>>& GetSpriteData() const noexcept;
        const std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>>& GetTextData() const noexcept;

        size_t GetResourceDescriptorCount() const noexcept;

    private:
        Scene& m_scene;
        const DeviceResources& m_deviceResources;

        std::wstring m_defaultDiffuseMap;
        std::wstring m_defaultNormalMap;

        UINT m_nextDescriptorHeapIndex;
        std::unique_ptr<DirectX::DescriptorHeap> m_pResourceDescriptors;
        std::unique_ptr<DirectX::CommonStates> m_pStates;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;

        std::unique_ptr<DirectX::BasicEffect> m_pOutlineEffect;
        std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_pOutlinePrimitiveBatch;

        std::unordered_map<std::shared_ptr<Material>, std::unique_ptr<DirectX::IEffect>>  m_materialData;
        std::unordered_map<std::shared_ptr<Model>,    std::unique_ptr<ModelDeviceData>>   m_modelData;
        std::unordered_map<std::shared_ptr<IMesh>,    std::unique_ptr<MeshDeviceData>>    m_meshData;
        std::unordered_map<std::wstring,              std::unique_ptr<TextureDeviceData>> m_textureData;
        std::unordered_map<std::shared_ptr<Sprite>,   std::unique_ptr<TextureDeviceData>> m_spriteData;
        std::unordered_map<std::shared_ptr<Text>,     std::unique_ptr<TextDeviceData>>    m_textData;  
};
