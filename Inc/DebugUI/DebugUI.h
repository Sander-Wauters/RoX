#pragma once

#include "RoX/Timer.h"
#include "RoX/Renderer.h"

class DebugUI {
    public:
        DebugUI(Timer& mainTimer, Renderer& renderer, std::shared_ptr<Scene> pScene, std::uint8_t debugAssetBatchIndex);

    public:
        void Show();

        void Add(std::shared_ptr<Scene> pScene);
        void RemoveScene(std::uint64_t GUID);

    private:
        void AddWorldGrid(AssetBatch& batch);
        void AddWorldAxis(AssetBatch& batch);

    public:
        std::shared_ptr<Scene> GetCurrentScene() const noexcept;
    
    private:
        Timer& m_mainTimer;
        Renderer& m_renderer;
        std::unordered_map<std::uint64_t, std::shared_ptr<Scene>> m_scenes;

        std::uint8_t m_debugAssetBatchIndex;
        std::uint64_t m_currentSceneGUID;
};

