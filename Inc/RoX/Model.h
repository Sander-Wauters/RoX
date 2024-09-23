#pragma once

#include <memory>
#include <vector>
#include <unordered_set>

#include "Asset.h"
#include "VertexTypes.h"
#include "Material.h"

// Forward declaration of **Model**.
class Model;

// Represents a node in the bone hierarchy.
class Bone : public Asset {
    public:
        static constexpr std::uint32_t INVALID_INDEX = std::uint32_t(-1);

        Bone(std::string name) noexcept;
        Bone(std::string name, std::uint32_t parentIndex) noexcept;

    public:
        struct aligned_deleter { void operator()(void* p) noexcept { _aligned_free(p); }};

        using TransformArray = std::unique_ptr<DirectX::XMMATRIX[], aligned_deleter>;

        static TransformArray MakeArray(std::uint64_t count) {
            void* temp = _aligned_malloc(sizeof(DirectX::XMMATRIX) * count, 16);
            if (!temp)
                throw std::bad_alloc();
            return TransformArray(static_cast<DirectX::XMMATRIX*>(temp));
        }

        std::uint32_t GetParentIndex() const noexcept;

        bool IsRoot() const noexcept;

        void SetParentIndex(std::uint32_t index) noexcept;

    private:
        // TODO: change this to std::uint8_t
        std::uint32_t m_parentIndex;
};

// A submesh determines to which set for vertices in a mesh a material is applied.
// Does not store it's material but an index into the array of materials in **Model**.
// The location of a submesh in world space is determined by the transformation in **m_instances**.
// There need to be 1 instance available at all time.
// If multiple instances need to be rendered then the **RenderFlags::Effect::Instances** should be set in the used material.
class Submesh : public Asset {
    public:
        Submesh(std::string name = "", std::uint32_t materialIndex = 0, bool visible = true) noexcept;

    public:
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
        // The last m_numberCulled instances in m_instances will not be sent to the GPU.
        std::uint32_t m_numCulled;
        std::vector<DirectX::XMFLOAT3X4> m_instances;

        std::uint32_t m_materialIndex; 
        std::uint32_t m_indexCount;
        std::uint32_t m_startIndex;
        std::uint32_t m_vertexOffset;

        bool m_visible;
};

class IMeshObserver {
    public:
        virtual void OnAdd(const std::unique_ptr<Submesh>& pSubmesh) = 0;

        virtual void OnRemoveSubmesh(std::uint8_t index) = 0;
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

        virtual void Add(std::unique_ptr<Submesh> pSubmesh) = 0;

        virtual void RemoveSubmesh(std::uint8_t index) = 0;

        virtual void Attach(IMeshObserver* pIMeshObserver) = 0;
        virtual void Detach(IMeshObserver* pIMeshObserver) noexcept = 0;

    public:
        virtual std::string GetName() const noexcept = 0;
        virtual std::uint64_t GetGUID() const noexcept = 0;

        virtual std::uint32_t GetBoneIndex() const noexcept = 0;
        virtual std::uint32_t GetNumSubmeshes() const noexcept = 0;
        virtual std::uint32_t GetNumVertices() const noexcept = 0;
        virtual std::uint32_t GetNumIndices() const noexcept = 0;

        virtual std::vector<std::uint32_t>& GetBoneInfluences() noexcept = 0;
        virtual std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept = 0;
        virtual std::vector<std::uint16_t>& GetIndices() noexcept = 0;

        virtual bool IsVisible() const noexcept = 0;

        virtual void SetName(std::string name) noexcept = 0;
        virtual void SetBoneIndex(std::uint32_t boneIndex) noexcept = 0;
        virtual void SetVisible(bool visible) noexcept = 0;
};

// Used in models that do not require skinned animations.
class Mesh : public IMesh, public Asset {
    public:
        Mesh(std::string name = "", bool visible = true) noexcept; 

    public:
        void Add(std::unique_ptr<Submesh> pSubmesh) override;

        void RemoveSubmesh(std::uint8_t index) override;

        void Attach(IMeshObserver* pIMeshObserver) override;
        void Detach(IMeshObserver* pIMeshObserver) noexcept override;

    public:
        std::string GetName() const noexcept override;
        std::uint64_t GetGUID() const noexcept override;

        std::uint32_t GetBoneIndex() const noexcept override;
        std::uint32_t GetNumSubmeshes() const noexcept override;
        std::uint32_t GetNumVertices() const noexcept override;
        std::uint32_t GetNumIndices() const noexcept override;

        std::vector<std::uint32_t>& GetBoneInfluences() noexcept override;
        std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept override;
        std::vector<VertexPositionNormalTexture>& GetVertices() noexcept;
        std::vector<std::uint16_t>& GetIndices() noexcept override;

        bool IsVisible() const noexcept override;

        void SetName(std::string name) noexcept override;
        void SetBoneIndex(std::uint32_t boneIndex) noexcept override;
        void SetVisible(bool visible) noexcept override;

    private:
        std::unordered_set<IMeshObserver*> m_iMeshObservers;

        std::uint32_t m_boneIndex;

        std::vector<std::uint32_t> m_boneInfluences;
        std::vector<std::unique_ptr<Submesh>> m_submeshes;
        std::vector<VertexPositionNormalTexture> m_vertices;
        std::vector<std::uint16_t> m_indices;

        bool m_visible;
};

// Used in models that require skinned animations.
class SkinnedMesh : public IMesh, public Asset {
    public:
        SkinnedMesh(std::string name = "", bool visible = true) noexcept; 

    public:
        void Add(std::unique_ptr<Submesh> pSubmesh) override;

        void RemoveSubmesh(std::uint8_t index) override;

        void Attach(IMeshObserver* pIMeshObserver) override;
        void Detach(IMeshObserver* pIMeshObserver) noexcept override;

    public:
        std::string GetName() const noexcept override;
        std::uint64_t GetGUID() const noexcept override;

        std::uint32_t GetBoneIndex() const noexcept override;
        std::uint32_t GetNumSubmeshes() const noexcept override;
        std::uint32_t GetNumVertices() const noexcept override;
        std::uint32_t GetNumIndices() const noexcept override;

        std::vector<std::uint32_t>& GetBoneInfluences() noexcept override;
        std::vector<std::unique_ptr<Submesh>>& GetSubmeshes() noexcept override;
        std::vector<VertexPositionNormalTextureSkinning>& GetVertices() noexcept;
        std::vector<std::uint16_t>& GetIndices() noexcept override;

        bool IsVisible() const noexcept override;

        void SetName(std::string name) noexcept override;
        void SetBoneIndex(std::uint32_t boneIndex) noexcept override;
        void SetVisible(bool visible) noexcept override;

    private:
        std::unordered_set<IMeshObserver*> m_iMeshObservers;

        std::uint32_t m_boneIndex;

        std::vector<std::uint32_t> m_boneInfluences;
        std::vector<std::unique_ptr<Submesh>> m_submeshes;
        std::vector<VertexPositionNormalTextureSkinning> m_vertices;
        std::vector<std::uint16_t> m_indices;

        bool m_visible;
};

class IModelObserver {
    public:
        virtual void OnAdd(const std::shared_ptr<Material>& pMaterial) = 0;
        virtual void OnAdd(const std::shared_ptr<IMesh>& pIMesh) = 0;

        virtual void OnRemoveMaterial(std::uint8_t index) = 0;
        virtual void OnRemoveIMesh(std::uint8_t index) = 0;
};

// Contains 1 or more meshes, all the materials used by there submeshes and a collection of tranformations to animate the model.
// All **Material**s need assigned to the **Model** before being added to a **Scene**.
class Model : public Asset {
    public:
        Model(std::shared_ptr<Material> pMaterial,
                std::string name = "", 
                bool visible = true);

        Model(Model& other);

        void Add(std::shared_ptr<Material> pMaterial);
        void Add(std::shared_ptr<IMesh> pMesh);

        void MakeBoneMatricesArray(std::uint64_t count);
        void MakeInverseBoneMatricesArray(std::uint64_t count);

        // Removes at the given index, only if there is more than one element.
        void RemoveMaterial(std::uint8_t index);
        void RemoveIMesh(std::uint8_t index);

        // Sets every instance of every submesh of every mesh to the given matrix.
        // Should only be used on models that don't use GPU instancing.
        void ApplyWorldTransform(DirectX::XMFLOAT3X4 W);

        void Attach(IModelObserver* pIModelObserver);
        void Detach(IModelObserver* pIModelObserver) noexcept;

    public:
        std::vector<std::shared_ptr<Material>>& GetMaterials() noexcept;
        std::vector<std::shared_ptr<IMesh>>& GetMeshes() noexcept;

        std::shared_ptr<Material>& GetMaterial(std::uint64_t GUID);
        std::shared_ptr<IMesh>& GetIMesh(std::uint64_t GUID);

        std::vector<Bone>& GetBones() noexcept;

        Bone::TransformArray& GetBoneMatrices() noexcept;
        Bone::TransformArray& GetInverseBindPoseMatrices() noexcept;

        bool IsVisible() const noexcept;

        // A model is skinned if it contains one material with the **Skinned** effect.
        bool IsSkinned() const noexcept;

        void SetVisible(bool visible) noexcept;

        std::uint32_t GetNumBones() const noexcept;
        std::uint32_t GetNumMeshes() const noexcept;
        std::uint32_t GetNumMaterials() const noexcept;
        std::uint32_t GetNumVertices() const noexcept;
        std::uint32_t GetNumIndices() const noexcept;
    
    private:
        std::unordered_set<IModelObserver*> m_modelObservers;

        std::vector<std::shared_ptr<Material>> m_materials;
        std::vector<std::shared_ptr<IMesh>> m_meshes;
        std::vector<Bone> m_bones;
        Bone::TransformArray m_boneMatrices;
        Bone::TransformArray m_inverseBindPoseMatrices;

        bool m_visible;
};
