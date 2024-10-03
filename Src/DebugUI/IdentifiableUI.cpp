#include "DebugUI/IdentifiableUI.h"

#include <stdexcept>

#include <ImGui/imgui.h>

#include "DebugUI/Util.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/GeneralUI.h"

void IdentifiableUI::Remover(AssetBatch::AssetType type, AssetBatch& batch) {
    static std::uint64_t GUID = 0;
    static bool GUIDnotFound = false;

    static char name[128] = "";
    static bool nameNotFound = false;

    ImGui::InputScalar(Util::GUIDLabel("GUID", "AssetRemover").c_str(), ImGuiDataType_U64, &GUID);
    ImGui::SameLine();
    if (ImGui::SmallButton(Util::GUIDLabel("Remove", "AssetRemover_GUID").c_str())) {
        GUIDnotFound = false;
        UpdateScheduler::Get().Add([&, type](){ 
            try {
                batch.Remove(type, GUID); 
            } catch (std::out_of_range ex) {
                GUIDnotFound = true;
            }
        });
    }
    GeneralUI::Error(GUIDnotFound, "GUID not found."); 

    ImGui::InputText(Util::GUIDLabel("Name", "AssetRemover").c_str(), name, std::size(name));
    ImGui::SameLine();
    if (ImGui::SmallButton(Util::GUIDLabel("Remove", "AssetRemover_Name").c_str())) {
        nameNotFound = false;
        UpdateScheduler::Get().Add([&, type](){ 
            try {
                batch.Remove(type, name);
            } catch (std::out_of_range ex) {
                nameNotFound = true;
            }
        });
    }
    GeneralUI::Error(nameNotFound, "Name not found."); 
}

void IdentifiableUI::Menu(Identifiable& asset) {
    static char name[128] = "";
    static bool editName = false;

    ImGui::Text("GUID: %llu", asset.GetGUID());

    ImGui::Text("Name: %s", asset.GetName().c_str());
    ImGui::SameLine();
    if (ImGui::SmallButton(Util::GUIDLabel("Change", asset.GetGUID()).c_str()))
        editName = !editName;

    if (editName) {
        ImGui::InputText(Util::GUIDLabel("", asset.GetGUID()).c_str(), name, std::size(name));
        ImGui::SameLine();
        if (ImGui::Button(Util::GUIDLabel("Save", asset.GetGUID()).c_str())) {
            asset.SetName(name);
            editName = false;
        }
    }
}

void IdentifiableUI::RemoverPopupMenu(AssetBatch::AssetType type, AssetBatch& batch) {
    std::string typeStr;
    switch (type) {
        case AssetBatch::AssetType::Material:
            typeStr = "Material";
            break;
        case AssetBatch::AssetType::Model:
            typeStr = "Model";
            break;
        case AssetBatch::AssetType::Sprite:
            typeStr = "Sprite";
            break;
        case AssetBatch::AssetType::Text:
            typeStr = "Text";
            break;
        case AssetBatch::AssetType::Outline:
            typeStr = "Outline";
            break;
    }

    if (ImGui::Button(Util::GUIDLabel("-", "AssetRemoverPopupMenu" + typeStr).c_str())) 
        ImGui::OpenPopup(std::string("AssetRemoverPopupMenu" + typeStr).c_str());
    if (ImGui::BeginPopup(std::string("AssetRemoverPopupMenu" + typeStr).c_str())) {
        Remover(type, batch);
        ImGui::EndPopup();
    }
}

