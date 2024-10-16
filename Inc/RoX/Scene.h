#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "Camera.h"
#include "Model.h"
#include "Sprite.h"
#include "Outline.h"
#include "AssetBatch.h"
#include "Identifiable.h"

// Contains all the data that will be rendered to the display.
// This data is stored in **AssetBatch**es.
// Assets are rendered bassed on their batch index. So batch 0 will be rendered first than batch 1 and so on.
class Scene : public Identifiable {
    public:
        Scene(const std::string name, Camera& camera) noexcept;
        ~Scene() noexcept;

    public:
        void Add(std::shared_ptr<AssetBatch> batch);
        void Add(std::uint8_t batch, std::shared_ptr<Material> pMaterial);
        void Add(std::uint8_t batch, std::shared_ptr<Model> pModel);
        void Add(std::uint8_t batch, std::shared_ptr<Sprite> pSprite);
        void Add(std::uint8_t batch, std::shared_ptr<Text> pText);
        void Add(std::uint8_t batch, std::shared_ptr<Outline> pOutline);

        void RemoveMaterial(std::uint8_t batch, std::uint64_t GUID);
        void RemoveModel(std::uint8_t batch, std::uint64_t GUID);
        void RemoveSprite(std::uint8_t batch, std::uint64_t GUID);
        void RemoveText(std::uint8_t batch, std::uint64_t GUID);
        void RemoveOutline(std::uint8_t batch, std::uint64_t GUID);

        // Uses **FindGUID** internally.
        void RemoveMaterial(std::uint8_t batch, std::string name);
        void RemoveModel(std::uint8_t batch, std::string name);
        void RemoveSprite(std::uint8_t batch, std::string name);
        void RemoveText(std::uint8_t batch, std::string name);
        void RemoveOutline(std::uint8_t batch, std::string name);

    public:
        Camera& GetCamera() const noexcept;

        std::shared_ptr<AssetBatch>& GetAssetBatch(std::uint8_t batch);
        std::vector<std::shared_ptr<AssetBatch>>& GetAssetBatches() noexcept;

        std::shared_ptr<Material>& GetMaterial(std::uint8_t batch, std::uint64_t GUID);
        std::shared_ptr<Model>& GetModel(std::uint8_t batch, std::uint64_t GUID);
        std::shared_ptr<Sprite>& GetSprite(std::uint8_t batch, std::uint64_t GUID);
        std::shared_ptr<Text>& GetText(std::uint8_t batch, std::uint64_t GUID);
        std::shared_ptr<Outline>& GetOutline(std::uint8_t batch, std::uint64_t GUID);

        // Uses **FindGUID** internally.
        std::shared_ptr<Material>& GetMaterial(std::uint8_t batch, std::string name);
        std::shared_ptr<Model>& GetModel(std::uint8_t batch, std::string name);
        std::shared_ptr<Sprite>& GetSprite(std::uint8_t batch, std::string name);
        std::shared_ptr<Text>& GetText(std::uint8_t batch, std::string name);
        std::shared_ptr<Outline>& GetOutline(std::uint8_t batch, std::string name);

        const Materials& GetMaterials(std::uint8_t batch) const;
        const Models& GetModels(std::uint8_t batch) const;
        const Sprites& GetSprites(std::uint8_t batch) const;
        const Texts& GetTexts(std::uint8_t batch) const;
        const Outlines& GetOutlines(std::uint8_t batch) const;

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
        Camera& m_camera;

        std::vector<std::shared_ptr<AssetBatch>> m_assetBatches;
};

