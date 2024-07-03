#pragma once

#include <ResourceUploadBatch.h>
#include <GraphicsMemory.h>
#include <GeometricPrimitive.h>
#include <Effects.h>
#include <SpriteFont.h>

#include "RoX/Model.h"

#include "Util/pch.h"
#include "SubmeshDeviceData.h"

struct ModelDeviceData;

struct TextureDeviceData {
    TextureDeviceData(std::uint32_t heapIndex) noexcept;

    std::uint32_t descriptorHeapIndex;
    Microsoft::WRL::ComPtr<ID3D12Resource> pTexture;
};

struct TextDeviceData {
    TextDeviceData(std::uint32_t heapIndex) noexcept;

    std::uint32_t descriptorHeapIndex;
    std::unique_ptr<DirectX::SpriteFont> pSpriteFont;
};

struct MeshDeviceData {
    MeshDeviceData() = default;

    std::vector<std::unique_ptr<SubmeshDeviceData>> submeshes;
};

struct ModelDeviceData {
    ModelDeviceData() = default;

    void DrawSkinned(ID3D12GraphicsCommandList* pCommandList, Model& model);
    void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory = false);

    std::vector<std::unique_ptr<DirectX::IEffect>*> effects;
    std::vector<std::shared_ptr<MeshDeviceData>> meshes;
};

