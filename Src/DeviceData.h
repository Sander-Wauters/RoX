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

// ---------------------------------------------------------------- //
//                            DEPRICATED
// ---------------------------------------------------------------- //
//namespace DeviceData {
//
//    struct Material {
//        Material() noexcept;
//
//        std::unique_ptr<DirectX::NormalMapEffect> pEffect;
//    };
//
//    struct OldMesh {
//        Material* pMaterialData = nullptr;
//        std::unique_ptr<DirectX::GeometricPrimitive> pGeometricPrimitive = nullptr;
//    };
//
//    struct Texture {
//        Texture(std::uint32_t heapIndex) noexcept;
//
//        std::uint32_t DescriptorHeapIndex;
//        Microsoft::WRL::ComPtr<ID3D12Resource> pTexture;
//    };
//
//    struct Text {
//        Text(std::uint32_t heapIndex) noexcept;
//
//        std::uint32_t DescriptorHeapIndex;
//        std::unique_ptr<DirectX::SpriteFont> pSpriteFont;
//    };
//
//    struct SubMesh {
//        SubMesh(
//                ID3D12Device* pDevice,
//                const std::vector<DirectX::VertexPositionNormalTexture>& vertices,
//                const std::vector<std::uint16_t>& indices
//               );
//
//        Material* pMaterialData;
//
//        std::uint32_t VertexStride;
//        std::uint32_t IndexBufferSize;
//        std::uint32_t VertexBufferSize;
//
//        D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
//        DXGI_FORMAT IndexFormat;
//        DirectX::SharedGraphicsResource IndexBuffer;
//        DirectX::SharedGraphicsResource VertexBuffer;
//        Microsoft::WRL::ComPtr<ID3D12Resource> StaticIndexBuffer;
//        Microsoft::WRL::ComPtr<ID3D12Resource> StaticVertexBuffer;
//        std::shared_ptr<std::vector<D3D12_INPUT_ELEMENT_DESC>> pVBDecl;
//
//        void DrawInstanced(
//                ID3D12GraphicsCommandList* pCommandList, 
//                std::uint32_t indexCount, 
//                std::uint32_t instanceCount, 
//                std::uint32_t startIndex, 
//                std::uint32_t vertexOffset, 
//                std::uint32_t startInstance = 0) const;
//    };
//
//    struct Mesh {
//        Mesh();
//
//        Material* pMaterialData;
//
//        std::vector<std::unique_ptr<SubMesh>> SubMeshes;
//        std::unique_ptr<DirectX::GeometricPrimitive> pGeometricPrimitive;
//
//        void Initialize(
//                ID3D12Device* pDevice,
//                std::uint16_t numSubMeshes,
//                const std::vector<DirectX::VertexPositionNormalTexture>& vertices,
//                const std::vector<std::uint16_t>& indices
//                );
//
//        // Load VB/IB resources for static geometry.
//        void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory);
//    };
//}
