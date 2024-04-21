#include "RoX/Mesh.h"

#include <stdexcept>

// ---------------------------------------------------------------- //
//                          Submesh
// ---------------------------------------------------------------- //

Submesh::Submesh(const std::string name, std::uint32_t materialIndex) 
    noexcept : m_name(name),
    m_materialIndex(materialIndex),
    m_indexCount(0),
    m_startIndex(0),
    m_vertexOffset(0),
    m_pVertices(std::make_shared<std::vector<DirectX::VertexPositionNormalTexture>>()),
    m_pIndices(std::make_shared<std::vector<std::uint16_t>>())
{}

std::string Submesh::GetName() const noexcept {
    return m_name;
}

std::uint32_t Submesh::GetMaterialIndex() const noexcept {
    return m_materialIndex;
}

std::uint32_t Submesh::GetIndexCount() const noexcept {
    return m_indexCount;
}

std::uint32_t Submesh::GetStartIndex() const noexcept {
    return m_startIndex;
}

std::uint32_t Submesh::GetVertexOffset() const noexcept {
    return m_vertexOffset;
}

std::shared_ptr<std::vector<DirectX::VertexPositionNormalTexture>> Submesh::GetVertices() noexcept {
    return m_pVertices;
}

std::shared_ptr<std::vector<std::uint16_t>> Submesh::GetIndices() noexcept {
    return m_pIndices;
}

void Submesh::SetMaterialIndex(std::uint32_t index) noexcept {
    m_materialIndex = index;
}

void Submesh::SetIndexCount(std::uint32_t count) noexcept {
    m_indexCount = count;
}

void Submesh::SetStartIndex(std::uint32_t index) noexcept {
    m_startIndex = index;
}

void Submesh::SetVertexOffset(std::uint32_t offset) noexcept {
    m_vertexOffset = offset;
}

// ---------------------------------------------------------------- //
//                          MeshPart
// ---------------------------------------------------------------- //

MeshPart::MeshPart(const std::string name) 
    noexcept : m_name(name)
{}

void MeshPart::Add(std::unique_ptr<Submesh> pSubmesh) noexcept {
    m_submeshes.push_back(std::move(pSubmesh));
}

std::string MeshPart::GetName() const noexcept {
    return m_name;
}

std::vector<std::unique_ptr<Submesh>>& MeshPart::GetSubmeshes() noexcept {
    return m_submeshes;
}

// ---------------------------------------------------------------- //
//                          Mesh
// ---------------------------------------------------------------- //

Mesh::Mesh(
        const std::string name, 
        std::shared_ptr<Material> pMaterial,
        bool visible) : 
    m_name(name),
    m_numberCulled(0),
    m_visible(visible)
{
    if (!pMaterial)
        throw std::runtime_error("Mesh '" + name + "' initialized without material");

    m_materials.push_back(std::move(pMaterial));

    std::shared_ptr<MeshPart> pMainPart = std::make_shared<MeshPart>(name + "_main_part");
    pMainPart->GetSubmeshes().push_back(std::make_unique<Submesh>(name + "_main_submesh", 0));
    m_meshParts.push_back(std::move(pMainPart));
}

void Mesh::Add(std::shared_ptr<Material> pMaterial) noexcept {
    m_materials.push_back(pMaterial); 
}

void Mesh::Add(std::shared_ptr<MeshPart> pMeshPart) noexcept {
    m_meshParts.push_back(pMeshPart);
}

std::string Mesh::GetName() const noexcept {
    return m_name;
}

std::uint64_t Mesh::GetNumberCulled() const noexcept {
    return m_numberCulled;
}

std::uint64_t Mesh::GetAmountOfVisibleInstances() const noexcept {
    return m_instances.size() - m_numberCulled;
}

std::vector<DirectX::XMFLOAT3X4>& Mesh::GetInstances() noexcept {
    return m_instances;
}

std::vector<std::shared_ptr<Material>>& Mesh::GetMaterials() noexcept {
    return m_materials;
}

std::vector<std::shared_ptr<MeshPart>>& Mesh::GetMeshParts() noexcept {
    return m_meshParts;
}

bool Mesh::IsVisible() const noexcept {
    return m_visible;
}

void Mesh::SetNumberCulled(std::uint64_t amount) {
    m_numberCulled = amount;
}

void Mesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

