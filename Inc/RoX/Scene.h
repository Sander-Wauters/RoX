#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"
#include "ISceneObserver.h"

class Scene {
    public:
        Scene(Camera& camera) noexcept;
        ~Scene() noexcept;

    public:
        void Add(std::shared_ptr<Mesh> pMesh);
        void Add(std::shared_ptr<Sprite> pSprite);
        void Add(std::shared_ptr<Text> pText);
        void Add(std::shared_ptr<Outline::Base> pOutline);

        void RemoveMesh(std::string name);
        void RemoveSprite(std::string name);
        void RemoveText(std::string name);
        void RemoveOutline(std::string name);

        void RegisterSceneObserver(ISceneObserver* pSceneObserver) noexcept;

        Camera& GetCamera() const noexcept;

        const std::shared_ptr<Mesh>& GetMesh(std::string name) const;
        const std::shared_ptr<Sprite>& GetSprite(std::string name) const;
        const std::shared_ptr<Text>& GetText(std::string name) const;
        const std::shared_ptr<Outline::Base>& GetOutline(std::string name) const;

        const std::unordered_map<std::string, std::shared_ptr<Mesh>>& GetMeshes() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Sprite>>& GetSprites() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Text>>& GetText() const noexcept;
        const std::unordered_map<std::string, std::shared_ptr<Outline::Base>>& GetOutlines() const noexcept;

        std::uint64_t GetTotalInstanceCount() const noexcept;
        std::uint64_t GetTotalVerticesLoaded() const noexcept;
        std::uint64_t GetTotalVerticesRendered() const noexcept;

    private:
        template<typename T> void NotifyAdd(std::shared_ptr<T> ptr) {
            for (ISceneObserver* sceneObserver : m_sceneObservers) {
                sceneObserver->OnAdd(ptr);
            }
        }

        template<typename T> void NotifyRemove(std::shared_ptr<T> ptr) {
            for (ISceneObserver* sceneObserver : m_sceneObservers) {
                sceneObserver->OnRemove(ptr);
            }
        }

    private:
        Camera& m_camera;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> m_meshes;
        std::unordered_map<std::string, std::shared_ptr<Sprite>> m_sprites;
        std::unordered_map<std::string, std::shared_ptr<Text>> m_text;
        std::unordered_map<std::string, std::shared_ptr<Outline::Base>> m_outlines;

        std::unordered_set<ISceneObserver*> m_sceneObservers;
};

