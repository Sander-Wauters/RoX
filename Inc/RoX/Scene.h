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
// This data is stored in static and dynamic **AssetBatch**es.
// The first **m_numStaticBatches** in **m_assetBatches** contain the static batches and 
// the following **m_numDynamicBatches** contain the dynamic batches.
// Static batches are used to hold data that will remain loaded for as long as the scene is.
// The assets containd in a static batch is guaranteed to be rendered before the assets in the dynamic batches.
// Assets should not be added or removed from these batches at runtime.
// Dynamic batches are used to hold assets that will either be added and/or removed at runtime.
class Scene {
    public:
        Scene(const std::string name, Camera& camera, std::uint8_t numStaticBatches, std::uint8_t numDynamicBatches) noexcept;
        ~Scene() noexcept;

    public:
        void Add(std::uint8_t batch, std::shared_ptr<Model> pModel);
        void Add(std::uint8_t batch, std::shared_ptr<Sprite> pSprite);
        void Add(std::uint8_t batch, std::shared_ptr<Text> pText);
        void Add(std::uint8_t batch, std::shared_ptr<IOutline> pOutline);

        void RemoveMesh(std::uint8_t batch, std::string name);
        void RemoveSprite(std::uint8_t batch, std::string name);
        void RemoveText(std::uint8_t batch, std::string name);
        void RemoveOutline(std::uint8_t batch, std::string name);

    public:
        std::string GetName() const noexcept;
        Camera& GetCamera() const noexcept;

        const std::vector<AssetBatch> GetAssetBatches() const noexcept;

        const std::shared_ptr<Model>& GetModel(std::uint8_t batch, std::string name) const;
        const std::shared_ptr<Sprite>& GetSprite(std::uint8_t batch, std::string name) const;
        const std::shared_ptr<Text>& GetText(std::uint8_t batch, std::string name) const;
        const std::shared_ptr<IOutline>& GetOutline(std::uint8_t batch, std::string name) const;

        const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels(std::uint8_t batch) const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Sprite>>& GetSprites(std::uint8_t batch) const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Text>>& GetTexts(std::uint8_t batch) const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<IOutline>>& GetOutlines(std::uint8_t batch) const noexcept;

        std::uint8_t GetNumAssetBatches() const noexcept;
        std::uint8_t GetNumStaticBatches() const noexcept;
        std::uint8_t GetNumDynamicBatches() const noexcept;
        std::uint8_t GetFirstDynamicBatchIndex() const noexcept;

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

        const std::uint8_t m_numStaticBatches;
        const std::uint8_t m_numDynamicBatches;
        std::vector<AssetBatch> m_assetBatches;
};

