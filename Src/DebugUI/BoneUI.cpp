#include "DebugUI/BoneUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/AssetUI.h"
#include "DebugUI/MathUI.h"

void BoneUI::Selector(std::uint32_t& index, std::vector<Bone>& bones) {
    ImGui::BeginChild("bone_hierarchy", ImVec2(ImGui::GetWindowWidth() - 15.f, 200.f), ImGuiChildFlags_None);
    for (std::uint32_t i = 0; i < bones.size(); ++i) {
        std::string text = std::to_string(i) + ": " + bones[i].GetName() + "; " + std::to_string(bones[i].GetParentIndex());
        if (ImGui::Selectable(text.c_str(), index == i))
            index = i;
    }
    ImGui::EndChild();
}

void BoneUI::Menu(Model& model, std::uint32_t boneIndex) {
    std::vector<Bone>& bones = model.GetBones();

    ImGui::SeparatorText("Identifiers");
    AssetUI::Menu(bones[boneIndex]);

    ImGui::SeparatorText("Parent");
    ImGui::Text("Parent: %s", bones[boneIndex].IsRoot() ? "NONE" : bones[bones[boneIndex].GetParentIndex()].GetName().c_str());
    ImGui::Text("Parent index: %d", bones[boneIndex].GetParentIndex());

    if (ImGui::CollapsingHeader("Transform")) {
        MathUI::AffineTransformation(model.GetBoneMatrices()[boneIndex]);
        ImGui::Separator();
        MathUI::Matrix(model.GetBoneMatrices()[boneIndex]);
    }

    if (ImGui::CollapsingHeader("Inverse bind pose")) {
        MathUI::AffineTransformation(model.GetBoneMatrices()[boneIndex]);
        ImGui::Separator();
        MathUI::Matrix(model.GetInverseBindPoseMatrices()[boneIndex]);
    }
}

