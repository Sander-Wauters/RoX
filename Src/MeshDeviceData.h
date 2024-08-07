#pragma once

#include "RoX/Model.h"

#include "SubmeshDeviceData.h"

class MeshDeviceData {
    public:
        MeshDeviceData(ID3D12Device* pDevice, IMesh* pMesh);

        void OnDeviceLost() noexcept;

        void PrepareForDraw(ID3D12GraphicsCommandList* pCommandList) const;

    public:
        std::vector<std::unique_ptr<SubmeshDeviceData>>& GetSubmeshes() noexcept;

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
        std::vector<std::unique_ptr<SubmeshDeviceData>> m_submeshes;

        std::uint32_t m_vertexStride;
        std::uint32_t m_indexBufferSize;
        std::uint32_t m_vertexBufferSize;

        D3D_PRIMITIVE_TOPOLOGY m_primitiveType;
        DXGI_FORMAT m_indexFormat;
        DirectX::SharedGraphicsResource m_indexBuffer;
        DirectX::SharedGraphicsResource m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_staticIndexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_staticVertexBuffer;
};
