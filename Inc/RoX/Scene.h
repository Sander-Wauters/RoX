#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "Camera.h"
#include "Model.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"
#include "AssetBatch.h"

// Contains all the data that will be rendered to the display.
// This data is stored in **AssetBatch**es.
// Assets are rendered bassed on their batch index. So batch 0 will be rendered first than batch 1 and so on.
class Scene {
    public:
        Scene(const std::string name, Camera& camera) noexcept;
        ~Scene() noexcept;

    public:
        void Add(std::shared_ptr<AssetBatch> batch);
        void Add(std::uint8_t batch, std::shared_ptr<Model> pModel);
        void Add(std::uint8_t batch, std::shared_ptr<Sprite> pSprite);
        void Add(std::uint8_t batch, std::shared_ptr<Text> pText);
        void Add(std::uint8_t batch, std::shared_ptr<Outline> pOutline);

        void RemoveMesh(std::uint8_t batch, std::string name);
        void RemoveSprite(std::uint8_t batch, std::string name);
        void RemoveText(std::uint8_t batch, std::string name);
        void RemoveOutline(std::uint8_t batch, std::string name);

    public:
        std::string GetName() const noexcept;
        Camera& GetCamera() const noexcept;

        std::shared_ptr<AssetBatch>& GetAssetBatch(std::uint8_t batch) noexcept;
        std::vector<std::shared_ptr<AssetBatch>>& GetAssetBatches() noexcept;

        std::shared_ptr<Model>& GetModel(std::uint8_t batch, std::string name);
        std::shared_ptr<Sprite>& GetSprite(std::uint8_t batch, std::string name);
        std::shared_ptr<Text>& GetText(std::uint8_t batch, std::string name);
        std::shared_ptr<Outline>& GetOutline(std::uint8_t batch, std::string name);

        const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels(std::uint8_t batch) const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Sprite>>& GetSprites(std::uint8_t batch) const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Text>>& GetTexts(std::uint8_t batch) const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Outline>>& GetOutlines(std::uint8_t batch) const noexcept;

        std::uint8_t GetNumAssetBatches() const noexcept;

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
        const std::string m_name;

        Camera& m_camera;

        std::vector<std::shared_ptr<AssetBatch>> m_assetBatches;
};

