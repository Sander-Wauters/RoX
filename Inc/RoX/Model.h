#pragma once

#include <unordered_map>
#include <memory>

#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"

#include "Material.h"

class Model;

class Bone {
    public:
        static constexpr std::uint32_t INVALID_INDEX = std::uint32_t(-1);

        Bone(const std::string name) noexcept;
        Bone(const std::string name, std::uint32_t parentIndex) noexcept;

    public:
        struct aligned_deleter { void operator()(void* p) noexcept { _aligned_free(p); }};

        using TransformArray = std::unique_ptr<DirectX::XMMATRIX[], aligned_deleter>;

        static TransformArray MakeArray(size_t count) {
            void* temp = _aligned_malloc(sizeof(DirectX::XMMATRIX) * count, 16);
            if (!temp)
                throw std::bad_alloc();
            return TransformArray(static_cast<DirectX::XMMATRIX*>(temp));
        }

        std::string GetName() const noexcept;

        std::uint32_t GetParentIndex() const noexcept;

        void SetParentIndex(std::uint32_t index) noexcept;

    private:
        const std::string m_name;

        std::uint32_t m_parentIndex;
};

class Submesh {
    public:
        Submesh(const std::string name, std::uint32_t materialIndex, bool visible = true) noexcept;

    public:
        std::string GetName() const noexcept;

        std::uint64_t GetNumCulled() const noexcept;
        std::uint64_t GetNumVisibleInstances() const noexcept;
        std::uint64_t GetNumVertices() const noexcept;
        std::uint64_t GetNumIndices() const noexcept;

        std::vector<DirectX::XMFLOAT3X4>& GetInstances() noexcept;
        
        std::uint32_t GetMaterialIndex() const noexcept;
        std::uint32_t GetIndexCount() const noexcept;
        std::uint32_t GetStartIndex() const noexcept;
        std::uint32_t GetVertexOffset() const noexcept;

        std::shared_ptr<Material> GetMaterial(Model* pModel) const;
        std::shared_ptr<std::vector<DirectX::VertexPositionNormalTexture>> GetVertices() const noexcept;
        std::shared_ptr<std::vector<std::uint16_t>> GetIndices() const noexcept;

        bool IsVisible() const noexcept;

        void SetNumberCulled(std::uint64_t amount);
        void SetMaterialIndex(std::uint32_t index) noexcept;
        void SetIndexCount(std::uint32_t count) noexcept;
        void SetStartIndex(std::uint32_t index) noexcept;
        void SetVertexOffset(std::uint32_t offset) noexcept;
        void SetVisible(bool visible) noexcept;

    private:
        const std::string m_name;

        // The last m_numberCulled instances in m_instances will not be sent to the GPU.
        std::uint64_t m_numCulled;
        std::vector<DirectX::XMFLOAT3X4> m_instances;

        std::uint32_t m_materialIndex; 
        std::uint32_t m_indexCount;
        std::uint32_t m_startIndex;
        std::uint32_t m_vertexOffset;

        std::shared_ptr<std::vector<DirectX::VertexPositionNormalTexture>> m_pVertices; 
        std::shared_ptr<std::vector<std::uint16_t>> m_pIndices;

        bool m_visible;
};

class Mesh {
    public:
        Mesh(const std::string name, bool visible = true) noexcept;

        void Add(std::unique_ptr<Submesh> pSubmesh) noexcept;

    public:
        std::string GetName() const noexcept;

        std::uint32_t GetBoneIndex() const noexcept;
        std::uint64_t GetNumSubmeshes() const noexcept;

        std::vector<std::uint32_t>& GetBoneInfluences() noexcept;
        std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept;

        bool IsVisible() const noexcept;

        void SetBoneIndex(std::uint32_t boneIndex) noexcept;
        void SetVisible(bool visible) noexcept;

    private:
        const std::string m_name;

        std::uint32_t m_boneIndex;

        std::vector<std::uint32_t> m_boneInfluences;
        std::vector<std::unique_ptr<Submesh>> m_submeshes;

        bool m_visible;
};

class Model {
    public:
        Model(const std::string name, 
                std::shared_ptr<Material> pMaterial,
                bool visible = true);

        Model(Model& other);

        void Add(std::shared_ptr<Material> pMaterial) noexcept;
        void Add(std::shared_ptr<Mesh> pMeshPart) noexcept;

    public:
        std::string GetName() const noexcept;

        std::uint64_t GetNumBones() const noexcept;
        std::uint64_t GetNumMeshes() const noexcept;

        std::vector<std::shared_ptr<Material>>& GetMaterials() noexcept;
        std::vector<std::shared_ptr<Mesh>>& GetMeshes() noexcept;
        std::vector<Bone>& GetBones() noexcept;
        Bone::TransformArray* GetBoneMatrices() noexcept;
        Bone::TransformArray* GetInverseBindPoseMatrices() noexcept;

        bool IsVisible() const noexcept;

        void SetVisible(bool visible) noexcept;
    
    private:
        const std::string m_name;

        std::vector<std::shared_ptr<Material>> m_materials;
        std::vector<std::shared_ptr<Mesh>> m_meshes;
        std::vector<Bone> m_bones;
        Bone::TransformArray m_boneMatrices;
        Bone::TransformArray m_inverseBindPoseMatrices;

        bool m_visible;
};
