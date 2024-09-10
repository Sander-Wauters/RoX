#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Model.h"
#include "Sprite.h"
#include "Outline.h"
#include "IAssetBatchObserver.h"

using Materials = std::unordered_map<std::uint64_t, std::shared_ptr<Material>>;
using Models    = std::unordered_map<std::uint64_t, std::shared_ptr<Model>>;
using Sprites   = std::unordered_map<std::uint64_t, std::shared_ptr<Sprite>>;
using Texts     = std::unordered_map<std::uint64_t, std::shared_ptr<Text>>;
using Outlines  = std::unordered_map<std::uint64_t, std::shared_ptr<Outline>>;

// Contains data that will be rendered to the display.
class AssetBatch {
    public:
        AssetBatch(const std::string name, bool visible = true, std::uint8_t maxAssets = 128) noexcept;
        ~AssetBatch() noexcept;

        bool operator== (const AssetBatch& other) const noexcept;

    public:
        std::uint64_t FindGUID(std::string name, Materials& materials);
        std::uint64_t FindGUID(std::string name, Models& models);
        std::uint64_t FindGUID(std::string name, Sprites& sprites);
        std::uint64_t FindGUID(std::string name, Texts& texts);
        std::uint64_t FindGUID(std::string name, Outlines& outlines);

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

        // Uses **FindGUID** internally.
        void RemoveMaterial(std::string name);
        void RemoveModel(std::string name);
        void RemoveSprite(std::string name);
        void RemoveText(std::string name);
        void RemoveOutline(std::string name);

        void RegisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept;
        void DeregisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept;

    public:
        std::string GetName() const noexcept;
        bool IsVisible() const noexcept;

        std::uint8_t GetMaxAssets() const noexcept;

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
        const std::string m_name;
        bool m_visible;

        const std::uint8_t m_maxAssets;

        Materials m_materials;
        Models m_models;
        Sprites m_sprites;
        Texts m_texts;
        Outlines m_outlines;

        std::unordered_set<IAssetBatchObserver*> m_assetBatchObservers;
};
