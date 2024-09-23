#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "RoX/Model.h"

#include "SubmeshDeviceData.h"
#include "DeviceResources.h"

class MeshDeviceData : public IDeviceObserver, public IMeshObserver {
    public:
        MeshDeviceData(DeviceResources& deviceResources, IMesh& iMesh);
        ~MeshDeviceData();

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void OnAdd(const std::unique_ptr<Submesh>& pSubmesh) override;

        void OnRemoveSubmesh(std::uint8_t index) override;
    
    public:
        void LoadIndexBuffer(IMesh* pIMesh);
        void LoadVertexBuffer(IMesh* pIMesh);

        void PrepareForDraw() const;

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
        DeviceResources& m_deviceResources;

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

        // Used as temporary storage for vertices and indices when restoring the device.
        // Outside of this use case these vectors should NOT store any data.
        std::vector<char> m_vertexData;
        std::vector<char> m_indexData;
};
