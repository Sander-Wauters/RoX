#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Model.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"
#include "IAssetBatchObserver.h"

// Contains data that will be rendered to the display.
class AssetBatch {
    public:
        AssetBatch(const std::string name, bool visible = true, std::uint8_t maxAssets = 128) noexcept;
        ~AssetBatch() noexcept;

        bool operator== (const AssetBatch& other) const noexcept;

    public:
        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);
        void Add(std::shared_ptr<Outline> pOutline);

        void RemoveModel(std::string name);
        void RemoveSprite(std::string name);
        void RemoveText(std::string name);
        void RemoveOutline(std::string name);

        void RegisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept;
        void DeRegisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept;

    public:
        std::string GetName() const noexcept;
        bool IsVisible() const noexcept;

        std::uint8_t GetMaxAssets() const noexcept;

        std::shared_ptr<Model>& GetModel(std::string name);
        std::shared_ptr<Sprite>& GetSprite(std::string name);
        std::shared_ptr<Text>& GetText(std::string name);
        std::shared_ptr<Outline>& GetOutline(std::string name);

        const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Sprite>>& GetSprites() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Text>>& GetTexts() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Outline>>& GetOutlines() const noexcept;

        std::uint64_t GetNumModels() const noexcept;
        std::uint64_t GetNumMeshes() const noexcept;
        std::uint64_t GetNumSubmeshes() const noexcept;
        std::uint64_t GetNumMaterials() const noexcept;
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

        std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
        std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
        std::unordered_map<std::string, std::shared_ptr<Text>> m_texts;
        std::unordered_map<std::string, std::shared_ptr<Outline>> m_outlines;

        std::unordered_set<IAssetBatchObserver*> m_assetBatchObservers;
};
