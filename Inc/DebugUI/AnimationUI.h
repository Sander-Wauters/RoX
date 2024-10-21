#pragma once

#include "RoX/Animation.h"
#include "RoX/Model.h"
#include "RoX/Scene.h"

#include "unordered_map"

// Wrapper class to provide the needed UI data for animations.
class AnimationInfo {
    public:
        AnimationInfo(Animation& animation);

    public:
        void Add(Model* pModel);
        void RemoveModel(std::uint64_t GUID);

    public:
        Animation& GetAnimation() noexcept;
        std::unordered_map<std::uint64_t, Model*>& GetModels() noexcept;

    private:
        Animation& m_animation;

        std::unordered_map<std::uint64_t, Model*> m_models;
};

namespace AnimationUI {
    void AvailableModels(AnimationInfo& animInfo, Scene& scene);

    void Selector(std::uint64_t& selectedGUID, std::unordered_map<std::uint64_t, std::shared_ptr<AnimationInfo>>& animationInfo);

    void Menu(std::unordered_map<std::uint64_t, std::shared_ptr<AnimationInfo>>& animationInfo, Scene& scene);
}

