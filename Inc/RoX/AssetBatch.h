#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Model.h"
#include "Sprite.h"
#include "Outline.h"
#include "Identifiable.h"

using Materials = std::unordered_map<std::uint64_t, std::shared_ptr<Material>>;
using Models    = std::unordered_map<std::uint64_t, std::shared_ptr<Model>>;
using Sprites   = std::unordered_map<std::uint64_t, std::shared_ptr<Sprite>>;
using Texts     = std::unordered_map<std::uint64_t, std::shared_ptr<Text>>;
using Outlines  = std::unordered_map<std::uint64_t, std::shared_ptr<Outline>>;

// Mainly used internally but can be used by the client to execute code when an asset gets added and/or removed.
class IAssetBatchObserver {
    public:
        virtual ~IAssetBatchObserver() = default;

        virtual void OnAdd(const std::shared_ptr<Material>& pMaterial) = 0;
        virtual void OnAdd(const std::shared_ptr<Model>& pModel) = 0;
        virtual void OnAdd(const std::shared_ptr<Sprite>& pSprite) = 0;
        virtual void OnAdd(const std::shared_ptr<Text>& pText) = 0;
        virtual void OnAdd(const std::shared_ptr<Outline>& pOutline) = 0;

        virtual void OnRemove(const std::shared_ptr<Material>& pMaterial) = 0;
        virtual void OnRemove(const std::shared_ptr<Model>& pModel) = 0;
        virtual void OnRemove(const std::shared_ptr<Sprite>& pSprite) = 0;
        virtual void OnRemove(const std::shared_ptr<Text>& pText) = 0;
        virtual void OnRemove(const std::shared_ptr<Outline>& pOutline) = 0;
};

// Contains data that will be rendered to the display.
class AssetBatch : public Identifiable {
    public:
        // Holds every type of asset that can be added and/or removed directly from an **AssetBatch**.
        // Primarily used to make the **Remove___** functions easier to use.
        enum class AssetType {
            Material,
            Model,
            Sprite,
            Text,
            Outline
        };

    public:
        AssetBatch(const std::string name, std::uint8_t maxNumUniqueTextures = 128, bool visible = true) noexcept;
        ~AssetBatch() noexcept;

        bool operator== (const AssetBatch& other) const noexcept;

    public:
        std::uint64_t FindGUID(std::string name, const Materials& materials);
        std::uint64_t FindGUID(std::string name, const Models& models);
        std::uint64_t FindGUID(std::string name, const Sprites& sprites);
        std::uint64_t FindGUID(std::string name, const Texts& texts);
        std::uint64_t FindGUID(std::string name, const Outlines& outlines);

        void Add(std::shared_ptr<Material> pMaterial);
        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);
        void Add(std::shared_ptr<Outline> pOutline);

        void RemoveMaterial(std::uint64_t GUID);
        void RemoveModel(std::uint64_t GUID);
        void RemoveSprite(std::uint64_t GUID);
        void RemoveText(std::uint64_t GUID);
        void RemoveOutline(std::uint64_t GUID);
        void Remove(AssetBatch::AssetType type, std::uint64_t GUID);

        // Uses **FindGUID** internally.
        void RemoveMaterial(std::string name);
        void RemoveModel(std::string name);
        void RemoveSprite(std::string name);
        void RemoveText(std::string name);
        void RemoveOutline(std::string name);
        void Remove(AssetBatch::AssetType type, std::string name);

        void Attach(IAssetBatchObserver* pIAssetBatchObserver);
        void Detach(IAssetBatchObserver* pIAssetBatchObserver) noexcept;

    private:
        void AddUniqueTexture(std::wstring texture);

    public:
        bool IsVisible() const noexcept;

        std::uint8_t GetMaxNumUniqueTextures() const noexcept;
        std::uint8_t GetNumUniqueTextures() const noexcept;

        std::shared_ptr<Material>& GetMaterial(std::uint64_t GUID);
        std::shared_ptr<Model>& GetModel(std::uint64_t GUID);
        std::shared_ptr<Sprite>& GetSprite(std::uint64_t GUID);
        std::shared_ptr<Text>& GetText(std::uint64_t GUID);
        std::shared_ptr<Outline>& GetOutline(std::uint64_t GUID);

        // Uses **FindGUID** internally.
        std::shared_ptr<Material>& GetMaterial(std::string name);
        std::shared_ptr<Model>& GetModel(std::string name);
        std::shared_ptr<Sprite>& GetSprite(std::string name);
        std::shared_ptr<Text>& GetText(std::string name);
        std::shared_ptr<Outline>& GetOutline(std::string name);

        const Materials& GetMaterials() const noexcept;
        const Models& GetModels() const noexcept;
        const Sprites& GetSprites() const noexcept;
        const Texts& GetTexts() const noexcept;
        const Outlines& GetOutlines() const noexcept;

        const std::unordered_set<std::wstring> GetUniqueTextures() noexcept;

        std::uint64_t GetNumMaterials() const noexcept;
        std::uint64_t GetNumModels() const noexcept;
        std::uint64_t GetNumMeshes() const noexcept;
        std::uint64_t GetNumSubmeshes() const noexcept;
        std::uint64_t GetNumSprites() const noexcept;
        std::uint64_t GetNumTexts() const noexcept;
        std::uint64_t GetNumOutlines() const noexcept;

        std::uint64_t GetNumSubmeshInstances() const noexcept;
        std::uint64_t GetNumRenderedSubmeshInstances() const noexcept;
        std::uint64_t GetNumLoadedVertices() const noexcept;
        std::uint64_t GetNumRenderedVertices() const noexcept;

        void SetVisible(bool visible);

    private:
        bool m_visible;

        const std::uint8_t m_maxNumUniqueTextures;

        Materials m_materials;
        Models m_models;
        Sprites m_sprites;
        Texts m_texts;
        Outlines m_outlines;

        std::unordered_set<std::wstring> m_uniqueTextures;
        std::unordered_set<IAssetBatchObserver*> m_assetBatchObservers;
};
