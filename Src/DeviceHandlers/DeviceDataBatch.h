#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "RoX/AssetBatch.h"
#include "RoX/IAssetBatchObserver.h"
#include "RoX/Material.h"
#include "RoX/Model.h"
#include "RoX/Sprite.h"

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

class DeviceDataBatch : public IDeviceObserver, public IAssetBatchObserver {
    public:
        DeviceDataBatch(DeviceResources& deviceResources, std::uint8_t descriptorHeapSize, const bool& msaaEnabled) noexcept;
        ~DeviceDataBatch() noexcept;

        DeviceDataBatch(DeviceDataBatch& other) noexcept;

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        // Used when loading in a new Scene.
        void Add(const AssetBatch& batch);
        void Add(std::shared_ptr<Material> pMaterial);
        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);

        // Used when a Scene is already loaded in.
        void OnAdd(std::shared_ptr<Material>& pMaterial) override;
        void OnAdd(std::shared_ptr<Model>& pModel) override;
        void OnAdd(std::shared_ptr<Sprite>& pSprite) override;
        void OnAdd(std::shared_ptr<Text>& pText) override;
        void OnAdd(std::shared_ptr<Outline>& pOutline) override;

        // Used when a Scene is already loaded in.
        void OnRemove(std::shared_ptr<Material>& pMaterial) override;
        void OnRemove(std::shared_ptr<Model>& pModel) override;
        void OnRemove(std::shared_ptr<Sprite>& pSprite) override;
        void OnRemove(std::shared_ptr<Text>& pText) override;
        void OnRemove(std::shared_ptr<Outline>& pOutline) override;

        void UpdateEffects(DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

        void CreateDeviceDependentResources();
        void CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateWindowSizeDependentResources();

    private:
        void CreateSpriteResource(ID3D12Device* pDevice, const std::shared_ptr<Sprite>& pSprite, std::unique_ptr<TextureDeviceData>& pSpriteData, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateTextResource(ID3D12Device* pDevice, const std::shared_ptr<Text>& pText, std::unique_ptr<TextDeviceData>& pTextData, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateTextureResource(ID3D12Device* pDevice, const std::wstring& fileName, std::unique_ptr<TextureDeviceData>& pTextureData, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateModelResource(ID3D12Device* pDevice, std::unique_ptr<ModelDeviceData>& pModelData, DirectX::ResourceUploadBatch& resourceUploadBatch);

        void CreateMaterialResource(ID3D12Device* pDevice, const std::shared_ptr<Material>& pMaterial, std::unique_ptr<DirectX::IEffect>& pIEffect, DirectX::RenderTargetState& renderTargetState);
        void CreateOutlineBatchResources(ID3D12Device* pDevice, DirectX::RenderTargetState& renderTargetState);

        std::unique_ptr<DirectX::IEffect> BuildEffect(Material& material) const;

        DirectX::RenderTargetState RenderTargetState() const noexcept;

        D3D12_INPUT_LAYOUT_DESC InputLayoutDesc(std::uint32_t flags) const;
        D3D12_BLEND_DESC BlendDesc(std::uint32_t flags) const noexcept;
        D3D12_DEPTH_STENCIL_DESC DepthStencilDesc(std::uint32_t flags) const noexcept;
        D3D12_RASTERIZER_DESC RasterizerDesc(std::uint32_t flags) const noexcept;
        D3D12_GPU_DESCRIPTOR_HANDLE SemplerDesc(std::uint32_t flags) const noexcept;

        bool CompareFileExtension(std::wstring filePath, std::wstring valid);
        void CreateTextureFromFile(std::wstring filePath, ID3D12Resource** ppTexture, DirectX::ResourceUploadBatch& resourceUploadBatch);

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

        std::uint8_t GetNumDescriptors() const noexcept;

    private:
        DeviceResources& m_deviceResources;
        const bool& m_msaaEnabled;

        const std::uint8_t m_descriptorHeapSize;
        std::uint8_t m_nextDescriptorHeapIndex;
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

