#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Camera.h"
#include "Model.h"
#include "Material.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"

class Scene {
    public:
        Scene(const std::string name, Camera& camera) noexcept;
        ~Scene() noexcept;

    public:
        void Add(std::shared_ptr<Model> pModel);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);
        void Add(std::shared_ptr<Outline::Base> pOutline);

        void RemoveMesh(std::string name);
        void RemoveSprite(std::string name);
        void RemoveText(std::string name);
        void RemoveOutline(std::string name);

        std::string GetName() const noexcept;
        Camera& GetCamera() const noexcept;

        const std::shared_ptr<Model>& GetModel(std::string name) const;
        const std::shared_ptr<Sprite>& GetSprite(std::string name) const;
        const std::shared_ptr<Text>& GetText(std::string name) const;
        const std::shared_ptr<Outline::Base>& GetOutline(std::string name) const;

        const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Sprite>>& GetSprites() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Text>>& GetText() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Outline::Base>>& GetOutlines() const noexcept;

        std::uint64_t GetNumInstances() const noexcept;
        std::uint64_t GetNumRenderedInstances() const noexcept;
        std::uint64_t GetNumLoadedVertices() const noexcept;
        std::uint64_t GetNumRenderedVertices() const noexcept;


    private:
        const std::string m_name;

        Camera& m_camera;

        std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
        std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
        std::unordered_map<std::string, std::shared_ptr<Text>> m_text;
        std::unordered_map<std::string, std::shared_ptr<Outline::Base>> m_outlines;
};

