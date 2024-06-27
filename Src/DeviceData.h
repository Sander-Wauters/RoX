#pragma once

#include <ResourceUploadBatch.h>
#include <GraphicsMemory.h>
#include <GeometricPrimitive.h>
#include <Effects.h>
#include <SpriteFont.h>

#include "RoX/Model.h"

#include "Util/pch.h"

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

struct SubmeshDeviceData {
    SubmeshDeviceData() noexcept;

    void PrepareForDraw(ID3D12GraphicsCommandList* pCommandList) const;

    void Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const;
    void Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh, DirectX::IEffect* pEffect) const;

    void DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const;
    void DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh, DirectX::IEffect* pEffect) const;

    DirectX::IEffect* GetEffect(ModelDeviceData* pModel, Submesh* pSubmesh) const;

    std::uint32_t vertexStride;
    std::uint32_t indexBufferSize;
    std::uint32_t vertexBufferSize;

    D3D_PRIMITIVE_TOPOLOGY primitiveType;
    DXGI_FORMAT indexFormat;
    DirectX::SharedGraphicsResource indexBuffer;
    DirectX::SharedGraphicsResource vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> staticIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> staticVertexBuffer;
    std::shared_ptr<std::vector<D3D12_INPUT_ELEMENT_DESC>> pVbDecl;
};

struct MeshDeviceData {
    MeshDeviceData() = default;

    std::vector<std::unique_ptr<SubmeshDeviceData>> submeshes;
};

struct ModelDeviceData {
    ModelDeviceData() = default;

    std::vector<std::unique_ptr<DirectX::IEffect>*> effects;
    std::vector<std::shared_ptr<MeshDeviceData>> meshes;

    void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory = false);
};

