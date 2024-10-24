#include "DebugUI/SceneUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/CameraUI.h"
#include "DebugUI/AssetBatchUI.h"

void SceneUI::Selector(std::uint64_t& selectedSceneGUID, std::unordered_map<std::uint64_t, std::shared_ptr<Scene>>& scenes) {
    for (auto& scenePair : scenes) {
        if (ImGui::Selectable(scenePair.second->GetName().c_str(), selectedSceneGUID == scenePair.first))
            selectedSceneGUID = scenePair.first;
    } 
}

void SceneUI::Stats(Scene& scene) {
    ImGui::Text("Batches:                    %d"  , scene.GetNumAssetBatches());

    ImGui::Separator();
    ImGui::Text("Models:                     %llu", scene.GetNumModels());
    ImGui::Text("Meshes:                     %llu", scene.GetNumMeshes());
    ImGui::Text("Submeshes:                  %llu", scene.GetNumSubmeshes());

    ImGui::Separator();
    ImGui::Text("Materials:                  %llu", scene.GetNumMaterials());
    ImGui::Text("Sprites:                    %llu", scene.GetNumSprites());
    ImGui::Text("Texts:                      %llu", scene.GetNumTexts());
    ImGui::Text("Outlines:                   %llu", scene.GetNumOutlines());

    ImGui::Separator();
    ImGui::Text("Submesh instances:          %llu", scene.GetNumSubmeshInstances());
    ImGui::Text("Rendered submesh instances: %llu", scene.GetNumRenderedSubmeshInstances());
    ImGui::Text("Loaded vertices:            %llu", scene.GetNumLoadedVertices());
    ImGui::Text("Rendered vertices:          %llu", scene.GetNumRenderedVertices());
}

void SceneUI::Menu(Scene& scene) {
    static std::uint8_t selectedBatch = 0;

    ImGui::SeparatorText("Batches");
    AssetBatchUI::Selector(selectedBatch, scene.GetAssetBatches());
    AssetBatchUI::Menu(*scene.GetAssetBatches()[selectedBatch]);

    ImGui::SeparatorText("Scene");
    if (ImGui::CollapsingHeader("Camera")) {
        CameraUI::Menu(scene.GetCamera());
        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Stats")) {
        Stats(scene);
        ImGui::Separator();
        ImGui::Spacing();
    }
}

