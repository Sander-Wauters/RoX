#pragma once

#include <unordered_map>
#include <memory>

#include "Model.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"

// Contains data that will be rendered to the display.
class AssetBatch {
    public:
        AssetBatch() noexcept;
        ~AssetBatch() noexcept;

    public:
        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);
        void Add(std::shared_ptr<IOutline> pOutline);

        void RemoveMesh(std::string name);
        void RemoveSprite(std::string name);
        void RemoveText(std::string name);
        void RemoveOutline(std::string name);

    public:
        const std::shared_ptr<Model>& GetModel(std::string name) const;
        const std::shared_ptr<Sprite>& GetSprite(std::string name) const;
        const std::shared_ptr<Text>& GetText(std::string name) const;
        const std::shared_ptr<IOutline>& GetOutline(std::string name) const;

        const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Sprite>>& GetSprites() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Text>>& GetTexts() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<IOutline>>& GetOutlines() const noexcept;

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

    private:
        std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
        std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
        std::unordered_map<std::string, std::shared_ptr<Text>> m_texts;
        std::unordered_map<std::string, std::shared_ptr<IOutline>> m_outlines;
};
