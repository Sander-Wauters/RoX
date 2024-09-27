#include "DebugUI/IMeshUI.h"

#include <ImGui/imgui.h>

#include <DebugUI/Util.h>
#include <DebugUI/UpdateScheduler.h>
#include <DebugUI/AssetUI.h>
#include <DebugUI/MathUI.h>
#include <DebugUI/SubmeshUI.h>
#include <DebugUI/MeshFactoryUI.h>

void IMeshUI::Selector(std::uint32_t& index, std::vector<std::shared_ptr<IMesh>>& meshes) {
    for (std::uint32_t i = 0; i < meshes.size(); ++i) {
        if (ImGui::Selectable(Util::GUIDLabel(meshes[i]->GetName(), meshes[i]->GetGUID()).c_str(), index == i, ImGuiSelectableFlags_DontClosePopups))
            index = i;
    }
}

void IMeshUI::Creator(Model& model) {
    static char name[128] = "";
    static bool visible = true;
    static bool skinned = false;

    ImGui::InputText("Name##MeshCreator", name, std::size(name));
    ImGui::Checkbox("Visible##MeshCreator", &visible);
    ImGui::Checkbox("Skinned##MeshCreator", &skinned);

    if (ImGui::Button("Create new mesh##MeshCreator")) {
        UpdateScheduler::Get().Add([&](){ 
            std::shared_ptr<IMesh> pIMesh;
            if (skinned)
                pIMesh = std::make_shared<SkinnedMesh>(name, visible);
            else
                pIMesh = std::make_shared<Mesh>(name, visible);
            pIMesh->Add(std::make_unique<Submesh>(std::string(name) + "_submesh"));
            model.Add(std::move(pIMesh)); 
        });
    }
}

void IMeshUI::Remover(Model& model) {
    std::uint32_t selected = std::uint32_t(-1);
    Selector(selected, model.GetMeshes());
    if (selected != std::uint32_t(-1))
        UpdateScheduler::Get().Add([&, selected](){ model.RemoveIMesh(selected); });
}

void IMeshUI::Menu(IMesh& iMesh) {
    bool visible = iMesh.IsVisible();
    if (ImGui::Checkbox(Util::GUIDLabel("Visible", iMesh.GetGUID()).c_str(), &visible))
        iMesh.SetVisible(visible);
    ImGui::SameLine();
    bool usingStaticBuffers = iMesh.IsUsingStaticBuffers();
    if (ImGui::Checkbox(Util::GUIDLabel("Using static buffers", iMesh.GetGUID()).c_str(), &usingStaticBuffers))
        UpdateScheduler::Get().Add([&, usingStaticBuffers](){ iMesh.UseStaticBuffers(usingStaticBuffers); });

    if (auto pMesh = dynamic_cast<Mesh*>(&iMesh)) {
        ImGui::SeparatorText("Identifiers");
        AssetUI::Menu(*pMesh);

        if (ImGui::CollapsingHeader("Vertices")) 
            MathUI::Vertices(pMesh->GetVertices());
    } else if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(&iMesh)) {
        ImGui::SeparatorText("Identifiers");
        AssetUI::Menu(*pSkinnedMesh);

        if (ImGui::CollapsingHeader("Vertices")) 
            MathUI::Vertices(pSkinnedMesh->GetVertices());

    }
    if (ImGui::CollapsingHeader("Indices"))
        MathUI::Indices(iMesh.GetIndices(), iMesh.GetNumVertices());
}

void IMeshUI::CreatorPopupMenu(Model& model) {
    if (ImGui::Button(Util::GUIDLabel("+", "IMeshCreatorPopupMenu").c_str()))
        ImGui::OpenPopup("IMeshCreatorPopupMenu");
    if (ImGui::BeginPopup("IMeshCreatorPopupMenu")) {
        Creator(model);
        ImGui::EndPopup();
    }
}

void IMeshUI::AddGeoOrSubmeshPopupMenu(Model& model, IMesh& iMesh) {
    if (ImGui::Button(Util::GUIDLabel("+", "IMeshAddGeoOrSubmeshPopupMenu").c_str()))
        ImGui::OpenPopup("IMeshAddGeoOrSubmeshPopupMenu");
    if (ImGui::BeginPopup("IMeshAddGeoOrSubmeshPopupMenu", ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu(Util::GUIDLabel("Add submesh", "IMeshAddGeoOrSubmeshPopupMenu").c_str())) {
                SubmeshUI::Creator(iMesh, model.GetMaterials());
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(Util::GUIDLabel("Add geometry", "IMeshAddGeoOrSubmeshPopupMenu").c_str())) {
                MeshFactoryUI::AddGeoToIMeshCreator(iMesh);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::EndPopup();
    }
}

void IMeshUI::RemoverPopupMenu(Model& model) {
    if (ImGui::Button(Util::GUIDLabel("-", "IMeshRemoverPopupMenu").c_str()))
        ImGui::OpenPopup("IMeshRemoverPopupMenu");
    if (ImGui::BeginPopup("IMeshRemoverPopupMenu")) {
        ImGui::SeparatorText("Remove mesh");
        Remover(model);
        ImGui::EndPopup();
    }
}

