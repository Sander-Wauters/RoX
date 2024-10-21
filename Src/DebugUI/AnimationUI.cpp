#include "DebugUI/AnimationUI.h"

#include "ImGui/imgui.h"

#include "DebugUI/Util.h"
#include "DebugUI/IdentifiableUI.h"

// ---------------------------------------------------------------- //
//                          AnimationInfo
// ---------------------------------------------------------------- //

AnimationInfo::AnimationInfo(Animation& animation) 
    : m_animation(animation)
{

}

void AnimationInfo::Add(Model* pModel) {
    m_models[pModel->GetGUID()] = pModel;
}

void AnimationInfo::RemoveModel(std::uint64_t GUID) {
    m_models.erase(GUID);
}

Animation& AnimationInfo::GetAnimation() noexcept {
    return m_animation;
}

std::unordered_map<std::uint64_t, Model*>& AnimationInfo::GetModels() noexcept {
    return m_models; 
}

// ---------------------------------------------------------------- //
//                          AnimationUI
// ---------------------------------------------------------------- //

void AnimationUI::AvailableModels(AnimationInfo& animInfo, Scene& scene) {
    if (ImGui::BeginListBox("##AvailableModels")) {
        for (auto& batch : scene.GetAssetBatches()) {
            const Models& models = batch->GetModels();

            for (auto& modelPair : models) {
                std::uint64_t GUID = modelPair.first;
                std::string name = modelPair.second->GetName();

                bool canApplyAnimation = animInfo.GetAnimation().GetNumBoneAnimations() == modelPair.second->GetNumBones();
                bool bound = animInfo.GetModels().find(GUID) != animInfo.GetModels().end();

                bool selected = canApplyAnimation && ImGui::Selectable(Util::GUIDLabel(name, GUID).c_str(), bound);

                if (selected && !bound)
                    animInfo.Add(modelPair.second.get());
                if (selected && bound)
                    animInfo.RemoveModel(GUID);
            }
        }
        ImGui::EndListBox();
    }
}

void AnimationUI::Selector(std::uint64_t& selectedGUID, std::unordered_map<std::uint64_t, std::shared_ptr<AnimationInfo>>& animationInfo) {
    if (ImGui::BeginListBox("##Animations")) {
        for (auto& animationInfoPair : animationInfo) {
            std::uint64_t GUID = animationInfoPair.first;
            std::string name = animationInfoPair.second->GetAnimation().GetName();
            if (ImGui::Selectable(Util::GUIDLabel(name, GUID).c_str(), selectedGUID == animationInfoPair.first, ImGuiSelectableFlags_None))
                selectedGUID = animationInfoPair.first;
        }
        ImGui::EndListBox();
    }
}

void AnimationUI::Menu(std::unordered_map<std::uint64_t, std::shared_ptr<AnimationInfo>>& animationInfo, Scene& scene) {
    static std::uint64_t selectedAnimationGUID = Identifiable::INVALID_GUID;

    ImGui::SeparatorText("Animations");
    Selector(selectedAnimationGUID, animationInfo);

    if (selectedAnimationGUID == Identifiable::INVALID_GUID)
        return;

    IdentifiableUI::Menu(animationInfo.at(selectedAnimationGUID)->GetAnimation());

    if (ImGui::CollapsingHeader("AvailableModels"))
        AvailableModels(*animationInfo.at(selectedAnimationGUID), scene);
}

