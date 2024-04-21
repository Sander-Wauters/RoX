#pragma once

#include <unordered_map>
#include <memory>

#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"

#include "Material.h"

class Submesh {
    public:
        Submesh(const std::string name, std::uint32_t materialIndex) noexcept;

    public:
        std::string GetName() const noexcept;
        
        std::uint32_t GetMaterialIndex() const noexcept;
        std::uint32_t GetIndexCount() const noexcept;
        std::uint32_t GetStartIndex() const noexcept;
        std::uint32_t GetVertexOffset() const noexcept;

        std::shared_ptr<std::vector<DirectX::VertexPositionNormalTexture>> GetVertices() noexcept;
        std::shared_ptr<std::vector<std::uint16_t>> GetIndices() noexcept;

        void SetMaterialIndex(std::uint32_t index) noexcept;
        void SetIndexCount(std::uint32_t count) noexcept;
        void SetStartIndex(std::uint32_t index) noexcept;
        void SetVertexOffset(std::uint32_t offset) noexcept;

    private:
        const std::string m_name;

        std::uint32_t m_materialIndex; 
        std::uint32_t m_indexCount;
        std::uint32_t m_startIndex;
        std::uint32_t m_vertexOffset;

        std::shared_ptr<std::vector<DirectX::VertexPositionNormalTexture>> m_pVertices; 
        std::shared_ptr<std::vector<std::uint16_t>> m_pIndices;
};

class MeshPart {
    public:
        MeshPart(const std::string name) noexcept;

        void Add(std::unique_ptr<Submesh> pSubmesh) noexcept;

    public:
        std::string GetName() const noexcept;

        std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept;

    private:
        const std::string m_name;

        std::vector<std::unique_ptr<Submesh>> m_submeshes;
};

class Mesh {
    public:
        Mesh(
                const std::string name, 
                std::shared_ptr<Material> pMaterial,
                bool visible = true
                );

        void Add(std::shared_ptr<Material> pMaterial) noexcept;
        void Add(std::shared_ptr<MeshPart> pMeshPart) noexcept;

    public:
        std::string GetName() const noexcept;

        std::uint64_t GetNumberCulled() const noexcept;
        std::uint64_t GetAmountOfVisibleInstances() const noexcept;

        std::vector<DirectX::XMFLOAT3X4>& GetInstances() noexcept;
        std::vector<std::shared_ptr<Material>>& GetMaterials() noexcept;
        std::vector<std::shared_ptr<MeshPart>>& GetMeshParts() noexcept;

        bool IsVisible() const noexcept;

        void SetNumberCulled(std::uint64_t amount);
        void SetVisible(bool visible) noexcept;
    
    private:
        const std::string m_name;

        // The last m_numberCulled instances in m_instances will not be sent to the GPU.
        std::uint64_t m_numberCulled;

        std::vector<DirectX::XMFLOAT3X4> m_instances;
        std::vector<std::shared_ptr<Material>> m_materials;
        std::vector<std::shared_ptr<MeshPart>> m_meshParts;

        bool m_visible;
};
