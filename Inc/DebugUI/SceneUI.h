#pragma once

#include <RoX/Scene.h>

namespace SceneUI {
    void Selector(std::uint64_t& selectedSceneGUID, std::unordered_map<std::uint64_t, std::shared_ptr<Scene>>& scenes);

    void Stats(Scene& scene);

    void Menu(Scene& scene);

}

