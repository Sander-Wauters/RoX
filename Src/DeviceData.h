#pragma once

#include <ResourceUploadBatch.h>
#include <GraphicsMemory.h>
#include <GeometricPrimitive.h>
#include <Effects.h>
#include <SpriteFont.h>

#include "Util/pch.h"

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

class SubmeshDeviceData {
    public:
        SubmeshDeviceData() noexcept;

        void Draw(ID3D12GraphicsCommandList* pCommandList) const;
        void DrawInstanced(
                ID3D12GraphicsCommandList* pCommandList, 
                std::uint32_t indexCount, 
                std::uint32_t instanceCount, 
                std::uint32_t startIndex, 
                std::uint32_t vertexOffset, 
                std::uint32_t startInstance = 0) const;

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

class MeshPartDeviceData {
    public:
        MeshPartDeviceData() = default;

        std::vector<std::unique_ptr<SubmeshDeviceData>> submeshes;
};

class MeshDeviceData {
    public:
        MeshDeviceData() = default;

        std::vector<std::unique_ptr<DirectX::IEffect>*> effects;
        std::vector<std::shared_ptr<MeshPartDeviceData>> meshParts;

        void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory = false);
};

