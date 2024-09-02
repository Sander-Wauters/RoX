#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "RoX/AssetBatch.h"
#include "RoX/Material.h"
#include "Rox/Model.h"
#include "RoX/Sprite.h"
#include "RoX/Text.h"

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

class DeviceDataBatch : public IDeviceObserver {
    public:
        DeviceDataBatch(DeviceResources& deviceResources) noexcept;
        ~DeviceDataBatch() noexcept;

        DeviceDataBatch(DeviceDataBatch& other) noexcept;

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void Add(const AssetBatch& batch);
        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);

        void UpdateEffects(DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

        void CreateDeviceDependentResources(bool msaaEnabled);
        void CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled);
        void CreateWindowSizeDependentResources();

    private:
        void CreateSpriteResource(ID3D12Device* pDevice, SpritePair& spritePair, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateTextResource(ID3D12Device* pDevice, TextPair& textPair, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateTextureResource(ID3D12Device* pDevice, TexturePair& texturePair, DirectX::ResourceUploadBatch& resourceUploadBatch);

        void CreateModelResource(ID3D12Device* pDevice, ModelPair& modelPair, DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateMaterialResource(ID3D12Device* pDevice, MaterialPair& materialPair, DirectX::RenderTargetState& renderTargetState);
        void CreateOutlineBatchResource(ID3D12Device* pDevice, DirectX::RenderTargetState& renderTargetState);

        D3D12_INPUT_LAYOUT_DESC InputLayout(std::uint32_t flags) const;
        D3D12_BLEND_DESC BlendDesc(std::uint32_t flags) const noexcept;
        D3D12_DEPTH_STENCIL_DESC DepthStencilDesc(std::uint32_t flags) const noexcept;
        D3D12_RASTERIZER_DESC RasterizerDesc(std::uint32_t flags) const noexcept;
        D3D12_GPU_DESCRIPTOR_HANDLE SemplerDesc(std::uint32_t flags) const noexcept;

        bool CompareFileExtension(std::wstring filePath, std::wstring valid);
        void CreateTextureFromFile(std::wstring filePath, ID3D12Resource** pTexture, DirectX::ResourceUploadBatch& resourceUploadBatch);

    public:
        bool HasMaterials() const noexcept;
        bool HasTextures() const noexcept;

        DirectX::DescriptorHeap* GetDescriptorHeap() const noexcept;
        DirectX::CommonStates* GetStates() const noexcept;
        DirectX::SpriteBatch* GetSpriteBatch() const noexcept;

        DirectX::BasicEffect* GetOutlineEffect() const noexcept;
        DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* GetOutlineBatch() const noexcept;

        const std::unordered_map<std::shared_ptr<Model>, std::unique_ptr<ModelDeviceData>>& GetModelData() const noexcept;
        const std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>>& GetSpriteData() const noexcept;
        const std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>>& GetTextData() const noexcept;

        std::uint64_t GetNumDescriptors() const noexcept;

    private:
        DeviceResources& m_deviceResources;

        std::uint32_t m_nextDescriptorHeapIndex;
        std::unique_ptr<DirectX::DescriptorHeap> m_pDescriptorHeap;
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

