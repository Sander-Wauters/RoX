#pragma once

#include <unordered_map>

#include <Effects.h>
#include <PrimitiveBatch.h>
#include <CommonStates.h>

#include "Util/pch.h"

#include "RoX/Scene.h"

#include "IDeviceObserver.h"
#include "DeviceResources.h"
#include "DeviceData.h"

class DeviceResourceData : public IDeviceObserver {
    public:
        DeviceResourceData(Scene& scene, const DeviceResources& deviceResources) noexcept;
        ~DeviceResourceData() noexcept;

        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void InitDataFromScene(std::shared_ptr<Model> pModel);
        void InitDataFromScene(std::shared_ptr<Sprite> pSprite);
        void InitDataFromScene(std::shared_ptr<Text> pText);

        void Update();

        bool HasMaterials() const noexcept;
        bool HasTextures() const noexcept;

        void BuildDeviceDependentResources(bool msaaEnabled);
        void BuildSprites(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildText(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTextures(DirectX::ResourceUploadBatch& resourceUploadBatch);

        void BuildRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled);
        void BuildModels(DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch);
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

        std::unordered_map<std::shared_ptr<Material>, std::unique_ptr<DirectX::IEffect>> m_materialData;
        std::unordered_map<std::shared_ptr<Model>, std::unique_ptr<ModelDeviceData>> m_modelData;
        std::unordered_map<std::shared_ptr<Mesh>, std::shared_ptr<MeshDeviceData>> m_meshData;
        std::unordered_map<std::wstring, std::unique_ptr<TextureDeviceData>> m_textureData;
        std::unordered_map<std::shared_ptr<Sprite>, std::unique_ptr<TextureDeviceData>> m_spriteData;
        std::unordered_map<std::shared_ptr<Text>, std::unique_ptr<TextDeviceData>> m_textData;  
};
