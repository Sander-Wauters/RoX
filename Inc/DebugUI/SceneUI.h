#pragma once

#include <RoX/Scene.h>

namespace SceneUI {
    void Stats(Scene& scene);

    void Menu(Scene& scene);

    void Selector(std::uint64_t& selectedSceneGUID, std::unordered_map<std::uint64_t, std::shared_ptr<Scene>>& scenes);

    void Window(Scene& scene, int windowFlags);
    void SelectorWindow(std::uint64_t& selectedSceneGUID, std::unordered_map<std::uint64_t, std::shared_ptr<Scene>>& scenes, int windowFlags);
}

