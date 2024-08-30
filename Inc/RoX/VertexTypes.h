#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3d12.h>

// Describes a vertex for use in static geometry.
struct VertexPositionNormalTexture {
    VertexPositionNormalTexture() = default;

    VertexPositionNormalTexture(const VertexPositionNormalTexture&) = default;
    VertexPositionNormalTexture& operator=(const VertexPositionNormalTexture&) = default;

    VertexPositionNormalTexture(VertexPositionNormalTexture&&) = default;
    VertexPositionNormalTexture& operator=(VertexPositionNormalTexture&&) = default;

    VertexPositionNormalTexture(
            const DirectX::XMFLOAT3& iposition, 
            const DirectX::XMFLOAT3& inormal, 
            const DirectX::XMFLOAT2& itextureCoordinate) noexcept
        : position(iposition),
        normal(inormal),
        textureCoordinate(itextureCoordinate) {}
    VertexPositionNormalTexture(
            DirectX::FXMVECTOR iposition, 
            DirectX::FXMVECTOR inormal, 
            DirectX::FXMVECTOR itextureCoordinate) noexcept 
    {
        DirectX::XMStoreFloat3(&this->position, iposition);
        DirectX::XMStoreFloat3(&this->normal, inormal);
        DirectX::XMStoreFloat2(&this->textureCoordinate, itextureCoordinate);
    }

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 textureCoordinate;

    static constexpr D3D12_INPUT_ELEMENT_DESC InputElements[] = {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
    };
    static constexpr D3D12_INPUT_ELEMENT_DESC InputElementsInstancing[] = {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,   0 },
        { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
    };
    static constexpr D3D12_INPUT_LAYOUT_DESC InputLayout = {
        InputElements, 3
    };
    static constexpr D3D12_INPUT_LAYOUT_DESC InputLayoutInstancing = {
        InputElementsInstancing, 6
    };
};

// Describes a vertex for use in skinned meshes.
struct VertexPositionNormalTextureSkinning {
    VertexPositionNormalTextureSkinning() = default;

    VertexPositionNormalTextureSkinning(const VertexPositionNormalTextureSkinning&) = default;
    VertexPositionNormalTextureSkinning& operator=(const VertexPositionNormalTextureSkinning&) = default;

    VertexPositionNormalTextureSkinning(VertexPositionNormalTextureSkinning&&) = default;
    VertexPositionNormalTextureSkinning& operator=(VertexPositionNormalTextureSkinning&&) = default;

    VertexPositionNormalTextureSkinning(
            const DirectX::XMFLOAT3& iposition, 
            const DirectX::XMFLOAT3& inormal, 
            const DirectX::XMFLOAT2& itextureCoordinate,
            const DirectX::XMUINT4&  iindices,
            const DirectX::XMFLOAT4& iweights) noexcept
        : position(iposition),
        normal(inormal),
        textureCoordinate(itextureCoordinate) 
    { 
        SetBlendIndices(iindices);
        SetBlendWeights(iweights);
    }

    VertexPositionNormalTextureSkinning(
            DirectX::FXMVECTOR iposition, 
            DirectX::FXMVECTOR inormal, 
            DirectX::FXMVECTOR itextureCoordinate,
            DirectX::XMUINT4 iindices,
            DirectX::FXMVECTOR iweights) noexcept 
    {
        DirectX::XMStoreFloat3(&this->position, iposition);
        DirectX::XMStoreFloat3(&this->normal, inormal);
        DirectX::XMStoreFloat2(&this->textureCoordinate, itextureCoordinate);
        SetBlendIndices(iindices);
        SetBlendWeights(iweights);
    }

    void SetBlendIndices(DirectX::XMUINT4 const& iindices) noexcept {
        indices = ((iindices.w & 0xff) << 24) | ((iindices.z & 0xff) << 16) | ((iindices.y & 0xff) << 8) | (iindices.x & 0xff);
    }

    void SetBlendWeights(DirectX::XMFLOAT4 const& iweights) noexcept { 
        weights = iweights;
    }
    void XM_CALLCONV SetBlendWeights(DirectX::FXMVECTOR iweights) noexcept {
        DirectX::XMStoreFloat4(&weights, iweights);
    }

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 textureCoordinate;
    std::uint32_t indices;
    DirectX::XMFLOAT4 weights;

    static constexpr D3D12_INPUT_ELEMENT_DESC InputElements[] = {
        { "SV_Position",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    static constexpr D3D12_INPUT_LAYOUT_DESC InputLayout = {
        InputElements, 5
    };
};
