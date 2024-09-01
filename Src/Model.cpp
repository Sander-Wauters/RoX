#include "RoX/Model.h"

#include "Util/pch.h"

// ---------------------------------------------------------------- //
//                          Bone
// ---------------------------------------------------------------- //

Bone::Bone(const std::string name) 
    noexcept : m_name(name),
    m_parentIndex(-1)
{}

Bone::Bone(const std::string name, std::uint32_t parentIndex) 
    noexcept : m_name(name),
    m_parentIndex(parentIndex)
{}

std::string Bone::GetName() const noexcept {
    return m_name;
}

std::uint32_t Bone::GetParentIndex() const noexcept {
    return m_parentIndex;
}

bool Bone::IsRoot() const noexcept {
    return m_parentIndex == INVALID_INDEX;
}

void Bone::SetParentIndex(std::uint32_t index) noexcept {
    m_parentIndex = index;
}

// ---------------------------------------------------------------- //
//                          Submesh
// ---------------------------------------------------------------- //

Submesh::Submesh(const std::string name, std::uint32_t materialIndex, bool visible) 
    noexcept : m_name(name),
    m_numCulled(0),
    m_instances({{
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f
            }}),
    m_materialIndex(materialIndex),
    m_indexCount(0),
    m_startIndex(0),
    m_vertexOffset(0),
    m_visible(visible)
{}

std::string Submesh::GetName() const noexcept {
    return m_name;
}

std::uint32_t Submesh::GetNumCulled() const noexcept {
    return m_numCulled;
}

std::uint32_t Submesh::GetNumInstances() const noexcept {
    return m_instances.size();
}

std::uint32_t Submesh::GetNumVisibleInstances() const noexcept {
    return m_instances.size() - m_numCulled;
}

std::vector<DirectX::XMFLOAT3X4>& Submesh::GetInstances() noexcept {
    return m_instances;
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

std::shared_ptr<Material> Submesh::GetMaterial(Model& grandParent) const {
    return grandParent.GetMaterials()[m_materialIndex];
}

bool Submesh::IsVisible() const noexcept {
    return m_visible;
}

void Submesh::SetNumberCulled(std::uint32_t amount) {
    if (amount > m_instances.size())
        throw std::invalid_argument("Amount is greater than m_instances.size()");
    m_numCulled = amount;
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

void Submesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          Mesh
// ---------------------------------------------------------------- //

Mesh::Mesh(const std::string name, bool visible) 
    noexcept : m_name(name),
    m_boneIndex(Bone::INVALID_INDEX),
    m_visible(visible)
{}

void Mesh::Add(std::unique_ptr<Submesh> pSubmesh) noexcept {
    m_submeshes.push_back(std::move(pSubmesh));
}

std::string Mesh::GetName() const noexcept {
    return m_name;
}

std::uint32_t Mesh::GetBoneIndex() const noexcept {
    return m_boneIndex;
}

std::uint32_t Mesh::GetNumSubmeshes() const noexcept {
    return m_submeshes.size();
}

std::uint32_t Mesh::GetNumVertices() const noexcept {
    return m_vertices.size();
}

std::uint32_t Mesh::GetNumIndices() const noexcept {
    return m_indices.size();
}

std::vector<std::uint32_t>& Mesh::GetBoneInfluences() noexcept {
    return m_boneInfluences;
}

std::vector<std::unique_ptr<Submesh>>& Mesh::GetSubmeshes() noexcept {
    return m_submeshes;
}

std::vector<VertexPositionNormalTexture>& Mesh::GetVertices() noexcept {
    return m_vertices;
}

std::vector<std::uint16_t>& Mesh::GetIndices() noexcept {
    return m_indices;
}

bool Mesh::IsVisible() const noexcept {
    return m_visible;
}

void Mesh::SetBoneIndex(std::uint32_t boneIndex) noexcept {
    m_boneIndex = boneIndex;
}

void Mesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          SkinnedMesh
// ---------------------------------------------------------------- //

SkinnedMesh::SkinnedMesh(const std::string name, bool visible) 
    noexcept : m_name(name),
    m_boneIndex(Bone::INVALID_INDEX),
    m_visible(visible)
{}

void SkinnedMesh::Add(std::unique_ptr<Submesh> pSubmesh) noexcept {
    m_submeshes.push_back(std::move(pSubmesh));
}

std::string SkinnedMesh::GetName() const noexcept {
    return m_name;
}

std::uint32_t SkinnedMesh::GetBoneIndex() const noexcept {
    return m_boneIndex;
}

std::uint32_t SkinnedMesh::GetNumSubmeshes() const noexcept {
    return m_submeshes.size();
}

std::uint32_t SkinnedMesh::GetNumVertices() const noexcept {
    return m_vertices.size();
}

std::uint32_t SkinnedMesh::GetNumIndices() const noexcept {
    return m_indices.size();
}

std::vector<std::uint32_t>& SkinnedMesh::GetBoneInfluences() noexcept {
    return m_boneInfluences;
}

std::vector<std::unique_ptr<Submesh>>& SkinnedMesh::GetSubmeshes() noexcept {
    return m_submeshes;
}

std::vector<VertexPositionNormalTextureSkinning>& SkinnedMesh::GetVertices() noexcept {
    return m_vertices;
}

std::vector<std::uint16_t>& SkinnedMesh::GetIndices() noexcept {
    return m_indices;
}

bool SkinnedMesh::IsVisible() const noexcept {
    return m_visible;
}

void SkinnedMesh::SetBoneIndex(std::uint32_t boneIndex) noexcept {
    m_boneIndex = boneIndex;
}

void SkinnedMesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          Model
// ---------------------------------------------------------------- //

Model::Model(
        const std::string name, 
        std::shared_ptr<Material> pMaterial,
        bool visible) : 
    m_name(name),
    m_visible(visible)
{
    if (!pMaterial)
        throw std::runtime_error("Mesh '" + name + "' initialized without material");

    m_materials.push_back(std::move(pMaterial));
}

Model::Model(Model& other) :
    m_name(other.GetName()),
    m_materials(other.GetMaterials()),
    m_meshes(other.GetMeshes()),
    m_bones(other.GetBones())
{
    if (other.GetNumBones() > 0) {
        if (other.GetBoneMatrices())
            m_boneMatrices = std::move(other.GetBoneMatrices());
        if (other.GetInverseBindPoseMatrices())
            m_inverseBindPoseMatrices = std::move(other.GetInverseBindPoseMatrices());
    }
}

void Model::Add(std::shared_ptr<Material> pMaterial) noexcept {
    m_materials.push_back(pMaterial); 
}

void Model::Add(std::shared_ptr<IMesh> pMesh) noexcept {
    m_meshes.push_back(pMesh);
}

void Model::MakeBoneMatricesArray(std::uint64_t count) {
    m_boneMatrices = Bone::MakeArray(count);
}

void Model::MakeInverseBoneMatricesArray(std::uint64_t count) {
    m_inverseBindPoseMatrices = Bone::MakeArray(count);
}

std::string Model::GetName() const noexcept {
    return m_name;
}

std::uint32_t Model::GetNumBones() const noexcept {
    return m_bones.size();
}

std::uint32_t Model::GetNumMeshes() const noexcept {
    return m_meshes.size();
}

std::uint32_t Model::GetNumMaterials() const noexcept {
    return m_materials.size();
}

std::vector<std::shared_ptr<Material>>& Model::GetMaterials() noexcept {
    return m_materials;
}

std::vector<std::shared_ptr<IMesh>>& Model::GetMeshes() noexcept {
    return m_meshes;
}

std::vector<Bone>& Model::GetBones() noexcept {
    return m_bones;
}

Bone::TransformArray& Model::GetBoneMatrices() noexcept {
    return m_boneMatrices;
}

Bone::TransformArray& Model::GetInverseBindPoseMatrices() noexcept {
    return m_inverseBindPoseMatrices;
}

bool Model::IsVisible() const noexcept {
    return m_visible;
}

bool Model::IsSkinned() const noexcept {
    bool isSkinned = true;
    for (const std::shared_ptr<IMesh>& pMesh : m_meshes) {
        auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(pMesh.get());
        if (!pSkinnedMesh)
            isSkinned = false;
    }
    return isSkinned;
}

void Model::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

void Model::SetWorldTransform(DirectX::XMFLOAT3X4 W) {
    for (std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        for (std::unique_ptr<Submesh>& pSubmesh : pIMesh->GetSubmeshes()) {
            for (DirectX::XMFLOAT3X4& instance : pSubmesh->GetInstances()) {
                instance = W;
            }
        }
    }
}

