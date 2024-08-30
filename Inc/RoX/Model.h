#pragma once

#include <memory>
#include <vector>

#include "VertexTypes.h"
#include "Material.h"

// Forward declaration of **Model**.
class Model;

// Represents a node in the bone hierarchy.
class Bone {
    public:
        static constexpr std::uint32_t INVALID_INDEX = std::uint32_t(-1);

        Bone(const std::string name) noexcept;
        Bone(const std::string name, std::uint32_t parentIndex) noexcept;

    public:
        struct aligned_deleter { void operator()(void* p) noexcept { _aligned_free(p); }};

        using TransformArray = std::unique_ptr<DirectX::XMMATRIX[], aligned_deleter>;

        static TransformArray MakeArray(std::uint64_t count) {
            void* temp = _aligned_malloc(sizeof(DirectX::XMMATRIX) * count, 16);
            if (!temp)
                throw std::bad_alloc();
            return TransformArray(static_cast<DirectX::XMMATRIX*>(temp));
        }

        std::string GetName() const noexcept;

        std::uint32_t GetParentIndex() const noexcept;

        bool IsRoot() const noexcept;

        void SetParentIndex(std::uint32_t index) noexcept;

    private:
        const std::string m_name;

        std::uint32_t m_parentIndex;
};

// A submesh determines to which set for vertices in a mesh a material is applied.
// Does not store it's material but an index into the array of materials in **Model**.
// The location of a submesh in world space is determined by the transformation in **m_instances**.
// There need to be 1 instance available at all time.
// If multiple instances need to be rendered then the **RenderFlags::Effect::Instances** should be set in the used material.
class Submesh {
    public:
        Submesh(const std::string name, std::uint32_t materialIndex, bool visible = true) noexcept;

    public:
        std::string GetName() const noexcept;

        std::uint32_t GetNumCulled() const noexcept;
        std::uint32_t GetNumInstances() const noexcept;
        std::uint32_t GetNumVisibleInstances() const noexcept;

        std::vector<DirectX::XMFLOAT3X4>& GetInstances() noexcept;
        
        std::uint32_t GetMaterialIndex() const noexcept;
        std::uint32_t GetIndexCount() const noexcept;
        std::uint32_t GetStartIndex() const noexcept;
        std::uint32_t GetVertexOffset() const noexcept;

        std::shared_ptr<Material> GetMaterial(Model& grandParent) const;

        bool IsVisible() const noexcept;

        void SetNumberCulled(std::uint32_t amount);
        void SetMaterialIndex(std::uint32_t index) noexcept;
        void SetIndexCount(std::uint32_t count) noexcept;
        void SetStartIndex(std::uint32_t index) noexcept;
        void SetVertexOffset(std::uint32_t offset) noexcept;
        void SetVisible(bool visible) noexcept;

    private:
        const std::string m_name;

        // The last m_numberCulled instances in m_instances will not be sent to the GPU.
        std::uint32_t m_numCulled;
        std::vector<DirectX::XMFLOAT3X4> m_instances;

        std::uint32_t m_materialIndex; 
        std::uint32_t m_indexCount;
        std::uint32_t m_startIndex;
        std::uint32_t m_vertexOffset;

        bool m_visible;
};

// Abstract interface representing any mesh which can be used by the **Renderer**.
// A mesh contains the vertices and indices of a part of a model.
class IMesh {
    protected:
        IMesh() = default;
        IMesh(IMesh&&) = default;
        IMesh& operator= (IMesh&&) = default;
        
    public:
        virtual ~IMesh() = default;

        virtual void Add(std::unique_ptr<Submesh> pSubmesh) noexcept = 0;

    public:
        virtual std::string GetName() const noexcept = 0;

        virtual std::uint32_t GetBoneIndex() const noexcept = 0;
        virtual std::uint32_t GetNumSubmeshes() const noexcept = 0;
        virtual std::uint32_t GetNumVertices() const noexcept = 0;
        virtual std::uint32_t GetNumIndices() const noexcept = 0;

        virtual std::vector<std::uint32_t>& GetBoneInfluences() noexcept = 0;
        virtual std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept = 0;
        virtual std::vector<std::uint16_t>& GetIndices() noexcept = 0;

        virtual bool IsVisible() const noexcept = 0;

        virtual void SetBoneIndex(std::uint32_t boneIndex) noexcept = 0;
        virtual void SetVisible(bool visible) noexcept = 0;
};

// Used in models that do not require skinned animations.
class Mesh : public IMesh {
    public:
        Mesh(const std::string name, bool visible = true) noexcept; 

        void Add(std::unique_ptr<Submesh> pSubmesh) noexcept override;

    public:
        std::string GetName() const noexcept override;

        std::uint32_t GetBoneIndex() const noexcept override;
        std::uint32_t GetNumSubmeshes() const noexcept override;
        std::uint32_t GetNumVertices() const noexcept override;
        std::uint32_t GetNumIndices() const noexcept override;

        std::vector<std::uint32_t>& GetBoneInfluences() noexcept override;
        std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept override;
        std::vector<VertexPositionNormalTexture>& GetVertices() noexcept;
        std::vector<std::uint16_t>& GetIndices() noexcept override;

        bool IsVisible() const noexcept override;

        void SetBoneIndex(std::uint32_t boneIndex) noexcept override;
        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;

        std::uint32_t m_boneIndex;

        std::vector<std::uint32_t> m_boneInfluences;
        std::vector<std::unique_ptr<Submesh>> m_submeshes;
        std::vector<VertexPositionNormalTexture> m_vertices;
        std::vector<std::uint16_t> m_indices;

        bool m_visible;
};

// Used in models that require skinned animations.
class SkinnedMesh : public IMesh {
    public:
        SkinnedMesh(const std::string name, bool visible = true) noexcept; 

        void Add(std::unique_ptr<Submesh> pSubmesh) noexcept override;

    public:
        std::string GetName() const noexcept override;

        std::uint32_t GetBoneIndex() const noexcept override;
        std::uint32_t GetNumSubmeshes() const noexcept override;
        std::uint32_t GetNumVertices() const noexcept override;
        std::uint32_t GetNumIndices() const noexcept override;

        std::vector<std::uint32_t>& GetBoneInfluences() noexcept override;
        std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept override;
        std::vector<VertexPositionNormalTextureSkinning>& GetVertices() noexcept;
        std::vector<std::uint16_t>& GetIndices() noexcept override;

        bool IsVisible() const noexcept override;

        void SetBoneIndex(std::uint32_t boneIndex) noexcept override;
        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;

        std::uint32_t m_boneIndex;

        std::vector<std::uint32_t> m_boneInfluences;
        std::vector<std::unique_ptr<Submesh>> m_submeshes;
        std::vector<VertexPositionNormalTextureSkinning> m_vertices;
        std::vector<std::uint16_t> m_indices;

        bool m_visible;
};

// Contains 1 or more meshes, all the materials used by there submeshes and a collection of tranformations to animate the model.
class Model {
    public:
        Model(const std::string name, 
                std::shared_ptr<Material> pMaterial,
                bool visible = true);

        Model(Model& other);

        void Add(std::shared_ptr<Material> pMaterial) noexcept;
        void Add(std::shared_ptr<IMesh> pMesh) noexcept;

        void MakeBoneMatricesArray(std::uint64_t count);
        void MakeInverseBoneMatricesArray(std::uint64_t count);

    public:
        std::string GetName() const noexcept;

        std::uint32_t GetNumBones() const noexcept;
        std::uint32_t GetNumMeshes() const noexcept;
        std::uint32_t GetNumMaterials() const noexcept;

        std::vector<std::shared_ptr<Material>>& GetMaterials() noexcept;
        std::vector<std::shared_ptr<IMesh>>& GetMeshes() noexcept;
        std::vector<Bone>& GetBones() noexcept;
        Bone::TransformArray& GetBoneMatrices() noexcept;
        Bone::TransformArray& GetInverseBindPoseMatrices() noexcept;

        bool IsVisible() const noexcept;
        bool IsSkinned() const noexcept;

        void SetVisible(bool visible) noexcept;
    
    private:
        const std::string m_name;

        std::vector<std::shared_ptr<Material>> m_materials;
        std::vector<std::shared_ptr<IMesh>> m_meshes;
        std::vector<Bone> m_bones;
        Bone::TransformArray m_boneMatrices;
        Bone::TransformArray m_inverseBindPoseMatrices;

        bool m_visible;
};
