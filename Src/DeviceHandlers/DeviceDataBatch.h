#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "RoX/AssetBatch.h"
#include "RoX/Material.h"
#include "RoX/Model.h"
#include "RoX/Sprite.h"

#include "DeviceResources.h"
#include "MaterialDeviceData.h"
#include "MeshDeviceData.h"
#include "ModelDeviceData.h"
#include "TextureDeviceData.h"
#include "TextDeviceData.h"
#include "IDeviceDataSupplier.h"

using MaterialPair = std::pair<const std::shared_ptr<Material>, std::shared_ptr<MaterialDeviceData>>;
using ModelPair    = std::pair<const std::shared_ptr<Model>,    std::unique_ptr<ModelDeviceData>>;
using MeshPair     = std::pair<const std::shared_ptr<IMesh>,    std::shared_ptr<MeshDeviceData>>; 
using TexturePair  = std::pair<const std::wstring,              std::shared_ptr<TextureDeviceData>>;
using SpritePair   = std::pair<const std::shared_ptr<Sprite>,   std::unique_ptr<TextureDeviceData>>;
using TextPair     = std::pair<const std::shared_ptr<Text>,     std::unique_ptr<TextDeviceData>>;

class DeviceDataBatch : public IDeviceObserver, public IAssetBatchObserver, public IDeviceDataSupplier {
    public:
        DeviceDataBatch(
                DeviceResources& deviceResources, 
                DirectX::CommonStates& commonStates,
                DirectX::RenderTargetState& rtState,
                std::uint8_t descriptorHeapSize) noexcept;
        ~DeviceDataBatch() noexcept;

        DeviceDataBatch(DeviceDataBatch& other) noexcept;

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void OnAdd(const std::shared_ptr<Material>& pMaterial) override;
        void OnAdd(const std::shared_ptr<Model>& pModel) override;
        void OnAdd(const std::shared_ptr<Sprite>& pSprite) override;
        void OnAdd(const std::shared_ptr<Text>& pText) override;
        void OnAdd(const std::shared_ptr<Outline>& pOutline) override;

        void OnRemove(const std::shared_ptr<Material>& pMaterial) override;
        void OnRemove(const std::shared_ptr<Model>& pModel) override;
        void OnRemove(const std::shared_ptr<Sprite>& pSprite) override;
        void OnRemove(const std::shared_ptr<Text>& pText) override;
        void OnRemove(const std::shared_ptr<Outline>& pOutline) override;

        std::shared_ptr<MaterialDeviceData> GetMaterialDeviceData(const std::shared_ptr<Material>& pMaterial) override;
        std::shared_ptr<MeshDeviceData> GetMeshDeviceData(const std::shared_ptr<IMesh>& pIMesh) override;

        void SignalMeshRemoved() override;

    public:
        // Used when loading in a new Scene.
        void Add(const AssetBatch& batch);
        
        void Update(DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

        void CreateDeviceDependentResources();
        void CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateWindowSizeDependentResources();

    private:
        std::uint8_t NextHeapIndex() noexcept;

        void CreateDescriptorHeapResources();
        void CreateSpriteBatchResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateOutlineBatchResources();

    public:
        bool HasMaterials() const noexcept;
        bool HasTextures() const noexcept;

        DirectX::DescriptorHeap* GetDescriptorHeap() const noexcept;
        DirectX::SpriteBatch* GetSpriteBatch() const noexcept;

        DirectX::BasicEffect* GetOutlineEffect() const noexcept;
        DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* GetOutlineBatch() const noexcept;

        const std::unordered_map<std::shared_ptr<Model>, std::unique_ptr<ModelDeviceData>>& GetModelData() const noexcept;
        const std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>>& GetSpriteData() const noexcept;
        const std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>>& GetTextData() const noexcept;

        std::uint8_t GetNumDescriptors() const noexcept;

    private:
        DeviceResources& m_deviceResources;
        DirectX::CommonStates& m_commonStates;
        DirectX::RenderTargetState& m_rtState;

        const std::uint8_t m_descriptorHeapSize;
        std::uint8_t m_nextDescriptorHeapIndex;
        std::queue<std::uint8_t> m_openDescriptorHeapIndices;

        std::unique_ptr<DirectX::DescriptorHeap> m_pDescriptorHeap;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;

        std::unique_ptr<DirectX::BasicEffect> m_pOutlineEffect;
        std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_pOutlinePrimitiveBatch;

        std::unordered_map<std::shared_ptr<Material>, std::shared_ptr<MaterialDeviceData>>  m_materialData;
        std::unordered_map<std::shared_ptr<Model>,    std::unique_ptr<ModelDeviceData>>     m_modelData;
        std::unordered_map<std::shared_ptr<IMesh>,    std::shared_ptr<MeshDeviceData>>      m_meshData;
        std::unordered_map<std::wstring,              std::shared_ptr<TextureDeviceData>>   m_textureData;
        std::unordered_map<std::shared_ptr<Sprite>,   std::unique_ptr<TextureDeviceData>>   m_spriteData;
        std::unordered_map<std::shared_ptr<Text>,     std::unique_ptr<TextDeviceData>>      m_textData;  
};

