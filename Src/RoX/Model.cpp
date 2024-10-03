#include "RoX/Model.h"

#include "../Util/pch.h"

// ---------------------------------------------------------------- //
//                          Bone
// ---------------------------------------------------------------- //

Bone::Bone(std::string name) 
    noexcept : Identifiable("bone", name),
    m_parentIndex(-1)
{}

Bone::Bone(const std::string name, std::uint32_t parentIndex) 
    noexcept : Identifiable("bone", name),
    m_parentIndex(parentIndex)
{}

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

Submesh::Submesh(std::string name, std::uint32_t materialIndex, bool visible) 
    noexcept : Identifiable("submesh", name),
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
//                          BaseMesh
// ---------------------------------------------------------------- //

BaseMesh::BaseMesh(std::string name, bool useStaticBuffers, bool visible) 
    noexcept : Identifiable("mesh", name),
    m_boneIndex(Bone::INVALID_INDEX),
    m_usingStaticBuffers(useStaticBuffers),
    m_visible(visible)
{}

void BaseMesh::UseStaticBuffers(bool useStaticBuffers) {
    m_usingStaticBuffers = useStaticBuffers;

    for (IMeshObserver* pIMeshObserver : m_iMeshObservers) {
        if (pIMeshObserver)
            pIMeshObserver->OnUseStaticBuffers(this, useStaticBuffers);
    }
}

void BaseMesh::UpdateBuffers() {
    for (IMeshObserver* pIMeshObserver : m_iMeshObservers) {
        if (pIMeshObserver)
            pIMeshObserver->OnUpdateBuffers(this);
    }
}

void BaseMesh::Add(std::unique_ptr<Submesh> pSubmesh) {
    if (!pSubmesh)
        throw std::invalid_argument("Submesh is nullptr");

    m_submeshes.push_back(std::move(pSubmesh));

    for (IMeshObserver* pIMeshObserver : m_iMeshObservers) {
        if (pIMeshObserver)
            pIMeshObserver->OnAdd(m_submeshes.back());
    }
}

void BaseMesh::RemoveSubmesh(std::uint8_t index) {
    if (m_submeshes.size() == 1 || index >= m_submeshes.size())
        return;

    for (IMeshObserver* pIMeshObserver : m_iMeshObservers) {
        if (pIMeshObserver)
            pIMeshObserver->OnRemoveSubmesh(index);
    }
    m_submeshes.erase(m_submeshes.begin() + index);
}

void BaseMesh::Attach(IMeshObserver* pIMeshObserver) {
    if (!pIMeshObserver)
        throw std::invalid_argument("IMeshObserver is nullptr.");
    m_iMeshObservers.insert(pIMeshObserver);
}

void BaseMesh::Detach(IMeshObserver* pIMeshObserver) noexcept {
    m_iMeshObservers.erase(pIMeshObserver);
}

std::string BaseMesh::GetName() const noexcept {
    return Identifiable::GetName();
}

std::uint64_t BaseMesh::GetGUID() const noexcept {
    return Identifiable::GetGUID();
}

std::uint32_t BaseMesh::GetBoneIndex() const noexcept {
    return m_boneIndex;
}

std::uint32_t BaseMesh::GetNumSubmeshes() const noexcept {
    return m_submeshes.size();
}

std::uint32_t BaseMesh::GetNumIndices() const noexcept {
    return m_indices.size();
}

std::vector<std::uint32_t>& BaseMesh::GetBoneInfluences() noexcept {
    return m_boneInfluences;
}

std::vector<std::unique_ptr<Submesh>>& BaseMesh::GetSubmeshes() noexcept {
    return m_submeshes;
}

std::vector<std::uint16_t>& BaseMesh::GetIndices() noexcept {
    return m_indices;
}

bool BaseMesh::IsUsingStaticBuffers() const noexcept {
    return m_usingStaticBuffers;
}

bool BaseMesh::IsVisible() const noexcept {
    return m_visible;
}

void BaseMesh::SetName(std::string name) noexcept {
    Identifiable::SetName(name);
}

void BaseMesh::SetBoneIndex(std::uint32_t boneIndex) noexcept {
    m_boneIndex = boneIndex;
}

void BaseMesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          Mesh
// ---------------------------------------------------------------- //

Mesh::Mesh(std::string name, bool useStaticBuffers, bool visible) 
    noexcept : BaseMesh(name, useStaticBuffers, visible)
{}

void Mesh::ClearGeometry() noexcept {
    m_indices.clear();
    m_vertices.clear();
}

void Mesh::RebuildFromBuffers() noexcept {
    for (IMeshObserver* pMeshObserver : m_iMeshObservers) {
        if (pMeshObserver)
            pMeshObserver->OnRebuildFromBuffers(this);
    }
}

std::vector<VertexPositionNormalTexture>& Mesh::GetVertices() noexcept {
    return m_vertices;
}

std::uint32_t Mesh::GetNumVertices() const noexcept {
    return m_vertices.size();
}

// ---------------------------------------------------------------- //
//                          SkinnedMesh
// ---------------------------------------------------------------- //

SkinnedMesh::SkinnedMesh(std::string name, bool useStaticBuffers, bool visible) 
    noexcept : BaseMesh(name, useStaticBuffers, visible)
{}

void SkinnedMesh::ClearGeometry() noexcept {
    m_indices.clear();
    m_vertices.clear();
}

void SkinnedMesh::RebuildFromBuffers() noexcept {
    for (IMeshObserver* pMeshObserver : m_iMeshObservers) {
        if (pMeshObserver)
            pMeshObserver->OnRebuildFromBuffers(this);
    }
}

std::vector<VertexPositionNormalTextureSkinning>& SkinnedMesh::GetVertices() noexcept {
    return m_vertices;
}

std::uint32_t SkinnedMesh::GetNumVertices() const noexcept {
    return m_vertices.size();
}

// ---------------------------------------------------------------- //
//                          Model
// ---------------------------------------------------------------- //

Model::Model(
        std::shared_ptr<Material> pMaterial,
        std::string name, 
        bool visible) : 
    Identifiable("model", name),
    m_visible(visible)
{
    if (!pMaterial)
        throw std::runtime_error("Model '" + GetName() + "' initialized without material");

    m_materials.push_back(std::move(pMaterial));
}

Model::Model(Model& other) : Identifiable(other),
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

void Model::UseStaticBuffers(bool useStaticBuffers) {
    for (std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        pIMesh->UseStaticBuffers(useStaticBuffers);
    }
}

void Model::Add(std::shared_ptr<Material> pMaterial) {
    if (!pMaterial)
        throw std::invalid_argument("Material was nullptr.");

    m_materials.push_back(pMaterial); 

    for (IModelObserver* pIModelObserver : m_modelObservers) {
        if (pIModelObserver)
            pIModelObserver->OnAdd(pMaterial);
    }
}

void Model::Add(std::shared_ptr<IMesh> pMesh) {
    if (!pMesh)
        throw std::invalid_argument("IMesh was nullptr.");

    m_meshes.push_back(pMesh);

    for (IModelObserver* pIModelObserver : m_modelObservers) {
        if (pIModelObserver)
            pIModelObserver->OnAdd(pMesh);
    }
}

void Model::ClearGeometry() noexcept {
    for (std::shared_ptr<IMesh>& pMesh : m_meshes) {
        pMesh->ClearGeometry();
    }
}

void Model::RebuildFromBuffers() noexcept {
    for (std::shared_ptr<IMesh>& pMesh : m_meshes) {
        pMesh->RebuildFromBuffers();
    }
}

void Model::MakeBoneMatricesArray(std::uint64_t count) {
    m_boneMatrices = Bone::MakeArray(count);
}

void Model::MakeInverseBoneMatricesArray(std::uint64_t count) {
    m_inverseBindPoseMatrices = Bone::MakeArray(count);
}

void Model::RemoveMaterial(std::uint8_t index) {
    if (m_materials.size() == 1 || index >= m_materials.size())
        return;

    for (IModelObserver* pIModelObserver : m_modelObservers) {
        if (pIModelObserver)
            pIModelObserver->OnRemoveMaterial(index);
    }

    m_materials.erase(m_materials.begin() + index);

    for (auto& pIMesh : m_meshes) {
        for (auto& pSubmesh : pIMesh->GetSubmeshes()) {
            if (pSubmesh->GetMaterialIndex() == index)
                pSubmesh->SetMaterialIndex(0);
        }
    }
}

void Model::RemoveIMesh(std::uint8_t index) {
    if (m_meshes.size() == 1 || index >= m_meshes.size())
        return;

    for (IModelObserver* pIModelObserver : m_modelObservers) {
        if (pIModelObserver)
            pIModelObserver->OnRemoveIMesh(index);
    }

    m_meshes.erase(m_meshes.begin() + index);
}

void Model::ApplyWorldTransform(DirectX::XMFLOAT3X4 W) {
    for (std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        for (std::unique_ptr<Submesh>& pSubmesh : pIMesh->GetSubmeshes()) {
            for (DirectX::XMFLOAT3X4& instance : pSubmesh->GetInstances()) {
                instance = W;
            }
        }
    }
}

void Model::Attach(IModelObserver* pIModelObserver) {
    if (!pIModelObserver)
        throw std::invalid_argument("IModelObserver is nullptr.");
    m_modelObservers.insert(pIModelObserver);
}

void Model::Detach(IModelObserver* pIModelObserver) noexcept {
    m_modelObservers.erase(pIModelObserver);
}


std::vector<std::shared_ptr<Material>>& Model::GetMaterials() noexcept {
    return m_materials;
}

std::vector<std::shared_ptr<IMesh>>& Model::GetMeshes() noexcept {
    return m_meshes;
}

std::shared_ptr<Material>& Model::GetMaterial(std::uint64_t GUID) {
    for (std::shared_ptr<Material>& pMaterial : m_materials) {
        if (pMaterial->GetGUID() == GUID)
            return pMaterial;
    }
    throw std::invalid_argument("No material with GUID: " + std::to_string(GUID));
}

std::shared_ptr<IMesh>& Model::GetIMesh(std::uint64_t GUID) {
    for (std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        if (pIMesh->GetGUID() == GUID)
            return pIMesh;
    }
    throw std::invalid_argument("No mesh with GUID: " + std::to_string(GUID));
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

bool Model::IsUsingStaticBuffers() const noexcept {
    std::uint8_t usingStaticBuffers = 0;
    for (const std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        usingStaticBuffers += pIMesh->IsUsingStaticBuffers();
    }
    return usingStaticBuffers == m_meshes.size();
}

bool Model::IsSkinned() const noexcept {
    for (const std::shared_ptr<Material>& pMaterial : m_materials) {
        if (pMaterial->GetFlags() & RenderFlags::Effect::Skinned)
            return true;
    }
    return false;
}

void Model::SetVisible(bool visible) noexcept {
    m_visible = visible;
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

std::uint32_t Model::GetNumVertices() const noexcept {
    std::uint32_t total = 0;
    for (const std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        total += pIMesh->GetNumVertices();
    }
    return total;
}

std::uint32_t Model::GetNumIndices() const noexcept {
    std::uint32_t total = 0;
    for (const std::shared_ptr<IMesh>& pIMesh : m_meshes) {
        total += pIMesh->GetNumIndices();
    }
    return total;
}

