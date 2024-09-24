#include "DebugUI/AssetBatchUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/AssetUI.h"
#include "DebugUI/GeneralUI.h"
#include "DebugUI/SpriteUI.h"
#include "DebugUI/TextUI.h"
#include "DebugUI/OutlineUI.h"
#include "DebugUI/SubmeshUI.h"
#include "DebugUI/IMeshUI.h"
#include "DebugUI/ModelUI.h"
#include "DebugUI/MaterialUI.h"
#include "DebugUI/Util.h"

void AssetBatchUI::Selector(std::uint8_t& index, std::vector<std::shared_ptr<AssetBatch>>& batches) {
    if (ImGui::BeginListBox("##Batches")) {
        for (std::uint8_t i = 0; i < batches.size(); ++i) {
            if (ImGui::Selectable(batches[i]->GetName().c_str(), index == i))
                index = i;
            if (index == i)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void AssetBatchUI::Stats(AssetBatch& batch) {
    ImGui::Text("Models:                     %llu", batch.GetNumModels());
    ImGui::Text("Meshes:                     %llu", batch.GetNumMeshes());
    ImGui::Text("Submeshes:                  %llu", batch.GetNumSubmeshes());

    ImGui::Separator();
    ImGui::Text("Max unique textures:        %d",   batch.GetMaxNumUniqueTextures());
    ImGui::Text("Unique textures:            %d",   batch.GetNumUniqueTextures());
    ImGui::Text("Materials:                  %llu", batch.GetNumMaterials());
    ImGui::Text("Sprites:                    %llu", batch.GetNumSprites());
    ImGui::Text("Texts:                      %llu", batch.GetNumTexts());
    ImGui::Text("Outlines:                   %llu", batch.GetNumOutlines());

    ImGui::Separator();
    ImGui::Text("Submesh instances:          %llu", batch.GetNumSubmeshInstances());
    ImGui::Text("Rendered submesh instances: %llu", batch.GetNumRenderedSubmeshInstances());
    ImGui::Text("Loaded vertices:            %llu", batch.GetNumLoadedVertices());
    ImGui::Text("Rendered vertices:          %llu", batch.GetNumRenderedVertices());
}

void AssetBatchUI::Menu(AssetBatch& batch) {
    int ID = 0;

    static Model* pSelectedModel = nullptr;
    static IMesh* pSelectedIMesh = nullptr;
    static Submesh* pSelectedSubmesh = nullptr;

    bool visible = batch.IsVisible();
    if (ImGui::Checkbox(Util::GUIDLabel("Visible", batch.GetName()).c_str(), &visible))
        batch.SetVisible(visible);

    if (ImGui::CollapsingHeader("Models")) {
        ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)ID++), ImVec2(ImGui::GetWindowWidth() - 15.f, 200.f), ImGuiChildFlags_None);
        ModelUI::CreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetUI::RemoverPopupMenu(AssetBatch::AssetType::Model, batch);
        ImGui::SameLine();
        ModelUI::Selector(batch, &pSelectedModel, &pSelectedIMesh, &pSelectedSubmesh);
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("%s > %s > %s", 
                pSelectedModel ? pSelectedModel->GetName().c_str() : "...", 
                pSelectedIMesh ? pSelectedIMesh->GetName().c_str() : "...",
                pSelectedSubmesh ? pSelectedSubmesh->GetName().c_str() : "...");
        ImGui::SameLine();
        GeneralUI::HelpMarker("Model > Mesh > Submesh");

        if (pSelectedSubmesh)
            SubmeshUI::Menu(*pSelectedSubmesh, pSelectedModel->GetMaterials());
        else if (pSelectedIMesh) {
            IMeshUI::AddGeoOrSubmeshPopupMenu(batch, *pSelectedModel, *pSelectedIMesh);
            ImGui::SameLine();
            SubmeshUI::RemoverPopupMenu(*pSelectedIMesh);
            ImGui::SameLine();
            IMeshUI::Menu(*pSelectedIMesh);
        }
        else if (pSelectedModel) {
            IMeshUI::CreatorPopupMenu(*pSelectedModel);
            ImGui::SameLine();
            IMeshUI::RemoverPopupMenu(*pSelectedModel);
            ImGui::SameLine();
            ModelUI::Menu(*pSelectedModel, batch.GetMaterials());
        }

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Materials")) {
        MaterialUI::CreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetUI::RemoverPopupMenu(AssetBatch::AssetType::Material, batch);

        MaterialUI::Menu(batch.GetMaterials());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Sprites")) {
        SpriteUI::CreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetUI::RemoverPopupMenu(AssetBatch::AssetType::Sprite, batch);

        SpriteUI::Menu(batch.GetSprites());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Text")) {
        TextUI::CreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetUI::RemoverPopupMenu(AssetBatch::AssetType::Text, batch);

        TextUI::Menu(batch.GetTexts());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Outlines")) {
        OutlineUI::CreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetUI::RemoverPopupMenu(AssetBatch::AssetType::Outline, batch);

        OutlineUI::Menu(batch.GetOutlines());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader(std::string("Stats##" + batch.GetName()).c_str())) {
        Stats(batch);

        ImGui::Separator();
        ImGui::Spacing();
    }
}

