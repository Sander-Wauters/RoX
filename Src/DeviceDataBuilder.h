#pragma once

#include <unordered_map>

#include <Effects.h>
#include <PrimitiveBatch.h>
#include <CommonStates.h>

#include "Util/pch.h"

#include "RoX/Scene.h"
#include "RoX/ISceneObserver.h"

#include "IDeviceObserver.h"
#include "DeviceResources.h"
#include "DeviceData.h"

class DeviceDataBuilder : public IDeviceObserver, public ISceneObserver {
    public:
        DeviceDataBuilder(Scene& scene, const DeviceResources& deviceResources) noexcept;
        ~DeviceDataBuilder() noexcept;

        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void OnAdd(Mesh* pMesh) override;
        void OnAdd(Sprite* pSprite) override;
        void OnAdd(Text* pText) override;
        void OnAdd(Outline::Base* pOutline) override;

        void OnRemove(Mesh* pMesh) override;
        void OnRemove(Sprite* pSprite) override;
        void OnRemove(Text* pText) override;
        void OnRemove(Outline::Base* pOutline) override;

        void Update();

        void BuildDeviceDependentResources(bool msaaEnabled);
        void BuildSpriteData(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTextData(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTextureData(DirectX::ResourceUploadBatch& resourceUploadBatch);

        void BuildRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled);
        void BuildMeshes(DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildMaterials(DirectX::RenderTargetState& renderTargetState);
        void BuildOutlines(DirectX::RenderTargetState& renderTargetState);

        void BuildWindowSizeDependentResources();

    private:
        bool CompareFileExtension(std::wstring filePath, std::wstring valid);
        void CreateTextureFromFile(std::wstring filePath, ID3D12Resource** pTexture, DirectX::ResourceUploadBatch& resourceUploadBatch);

    public:
        Scene& GetScene() const noexcept;

        DirectX::DescriptorHeap* GetDescriptorHeap() const noexcept;
        DirectX::CommonStates* GetStates() const noexcept;
        DirectX::SpriteBatch* GetSpriteBatch() const noexcept;

        DirectX::BasicEffect* GetOutlineEffect() const noexcept;
        DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* GetOutlineBatch() const noexcept;

        const std::unordered_map<Mesh*, std::unique_ptr<DeviceData::Mesh>>& GetMeshData() const noexcept;
        const std::unordered_map<const Material*, std::unique_ptr<DeviceData::Material>>& GetMaterialData() const noexcept;
        const std::unordered_map<Sprite*, std::unique_ptr<DeviceData::Texture>>& GetSpriteData() const noexcept;
        const std::unordered_map<Text*, std::unique_ptr<DeviceData::Text>>& GetTextData() const noexcept;

        DeviceData::Material* GetMaterialData(const Material* pMaterial) const;
        DeviceData::Texture* GetTextureData(std::wstring filePath) const;

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
        
        std::unordered_map<Mesh*, std::unique_ptr<DeviceData::Mesh>> m_meshData;
        std::unordered_map<const Material*, std::unique_ptr<DeviceData::Material>> m_materialData;
        std::unordered_map<Sprite*, std::unique_ptr<DeviceData::Texture>> m_spriteData;
        std::unordered_map<Text*, std::unique_ptr<DeviceData::Text>> m_textData;

        std::unordered_map<std::wstring, std::unique_ptr<DeviceData::Texture>> m_textureData;
        
};
