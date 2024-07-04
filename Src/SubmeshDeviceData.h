#pragma once

#include <GraphicsMemory.h>
#include <Effects.h>
#include <ResourceUploadBatch.h>

#include "RoX/Model.h"

#include "Util/pch.h"

class SubmeshDeviceData {
    public:
        SubmeshDeviceData(ID3D12Device* pDevice, Submesh* pSubmesh);

        void OnDeviceLost() noexcept;

        void PrepareForDraw(ID3D12GraphicsCommandList* pCommandList) const;

        void Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const;
        void DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const;

    public:
        std::uint32_t GetIndexBufferSize() const noexcept;
        std::uint32_t GetVertexBufferSize() const noexcept;

        DirectX::SharedGraphicsResource& GetIndexBuffer() noexcept;
        DirectX::SharedGraphicsResource& GetVertexBuffer() noexcept;

        Microsoft::WRL::ComPtr<ID3D12Resource>& GetStaticIndexBuffer() noexcept;
        Microsoft::WRL::ComPtr<ID3D12Resource>& GetStaticVertexBuffer() noexcept;

        void SetIndexBufferSize(std::uint32_t size) noexcept;
        void SetVertexBufferSize(std::uint32_t size) noexcept;

        void SetStaticIndexBuffer(ID3D12Resource* pIndexBuffer);
        void SetStaticVertexBuffer(ID3D12Resource* pVertexBuffer);

    private:
        std::uint32_t m_vertexStride;
        std::uint32_t m_indexBufferSize;
        std::uint32_t m_vertexBufferSize;

        D3D_PRIMITIVE_TOPOLOGY m_primitiveType;
        DXGI_FORMAT m_indexFormat;
        DirectX::SharedGraphicsResource m_indexBuffer;
        DirectX::SharedGraphicsResource m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_staticIndexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_staticVertexBuffer;
        std::shared_ptr<std::vector<D3D12_INPUT_ELEMENT_DESC>> m_pVbDecl;
};
