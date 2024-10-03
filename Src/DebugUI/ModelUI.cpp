#include "DebugUI/ModelUI.h"

#include <ImGui/imgui.h>

#include "RoX/AssetIO.h"

#include "DebugUI/Util.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/MeshFactoryUI.h"
#include "DebugUI/AssetUI.h"
#include "DebugUI/GeneralUI.h"
#include "DebugUI/MaterialUI.h"
#include "DebugUI/MathUI.h"
#include "DebugUI/BoneUI.h"

void ModelUI::Selector(AssetBatch& batch, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh) {
    int expandAll = -1;
    if (ImGui::Button("Expand all"))
        expandAll = 1;
    ImGui::SameLine();
    if (ImGui::Button("Collapse all"))
        expandAll = 0;

    static ImGuiTreeNodeFlags hierarchyBaseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    for (auto modelPair : batch.GetModels()) {
        ImGuiTreeNodeFlags hierarchyNodeFlags = hierarchyBaseNodeFlags;

        if (expandAll != -1)
            ImGui::SetNextItemOpen(expandAll != 0);
        bool modelNodeOpen = ImGui::TreeNodeEx(modelPair.second->GetName().c_str(), hierarchyNodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            *ppSelectedModel = modelPair.second.get();
            *ppSelectedIMesh = nullptr;
            *ppSelectedSubmesh = nullptr;
        }

        if (modelNodeOpen) {
            for (std::shared_ptr<IMesh>& pIMesh : modelPair.second->GetMeshes()) {
                if (expandAll != -1)
                    ImGui::SetNextItemOpen(expandAll != 0);
                bool meshNodeOpen = ImGui::TreeNodeEx(pIMesh->GetName().c_str(), hierarchyNodeFlags);

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                    *ppSelectedModel = modelPair.second.get();
                    *ppSelectedIMesh = pIMesh.get();
                    *ppSelectedSubmesh = nullptr;
                }

                if (meshNodeOpen) {
                    for (std::unique_ptr<Submesh>& pSubmesh : pIMesh->GetSubmeshes()) {
                        hierarchyNodeFlags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                        ImGui::TreeNodeEx(pSubmesh->GetName().c_str(), hierarchyNodeFlags);
                        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                            *ppSelectedModel = modelPair.second.get();
                            *ppSelectedIMesh = pIMesh.get();
                            *ppSelectedSubmesh = pSubmesh.get();
                        }
                    }
                    hierarchyNodeFlags = hierarchyBaseNodeFlags;

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}

void ModelUI::Creator(AssetBatch& batch) {
    static char name[128] = "";
    static MeshFactory::Geometry geo = MeshFactory::Geometry::Cube;
    static char filePath[128] = "";
    static std::uint64_t baseMaterialGUID = Asset::INVALID_GUID;
    static bool visible = true;
    static bool skinned = false;
    static bool packed = true;

    ImGui::InputText("Name##ModelCreator", name, std::size(name));
    ImGui::Checkbox("Visible##ModelCreator", &visible);
    ImGui::Checkbox("Skinned##ModelCreator", &skinned);
    MeshFactoryUI::GeoSelector(geo);

    ImGui::SeparatorText("Material");
    MaterialUI::Selector(baseMaterialGUID, batch.GetMaterials());
    GeneralUI::Error(baseMaterialGUID == Asset::INVALID_GUID, "Must select a material.");

    ImGui::SeparatorText("Import from file (optional)");
    bool validModel = GeneralUI::InputFilePath("Filepath##ModelCreator", filePath, std::size(filePath)); 
    GeneralUI::Error(validModel, "Invalid file path");
    ImGui::Checkbox("Pack meshes##ModelCreator", &packed);

    if (ImGui::Button("Create new model##ModelCreator") && (baseMaterialGUID != Asset::INVALID_GUID)) {
        if (validModel) {
            UpdateScheduler::Get().Add([&](){ 
                std::shared_ptr<Model> pModel;
                pModel = AssetIO::ImportModel(filePath, batch.GetMaterial(baseMaterialGUID), skinned, packed);
                batch.Add(std::move(pModel)); 
            });
        } else {
            UpdateScheduler::Get().Add([&](){ 
                std::shared_ptr<Model> pModel;

                pModel = std::make_unique<Model>(
                        batch.GetMaterial(baseMaterialGUID),
                        std::string(name),
                        visible);

                std::shared_ptr<IMesh> pIMesh;
                if (skinned)
                    pIMesh = std::make_shared<SkinnedMesh>();
                else
                    pIMesh = std::make_shared<Mesh>();

                MeshFactory::Add(geo, *pIMesh);

                pModel->Add(std::move(pIMesh));

                batch.Add(std::move(pModel)); 
            });
        }
    }
}

void ModelUI::Menu(Model& model, const Materials& availableMaterials) {
    bool visible = model.IsVisible();
    if (ImGui::Checkbox(Util::GUIDLabel("Visible", model.GetGUID()).c_str(), &visible))
        model.SetVisible(visible);
    ImGui::SameLine();
    bool usingStaticBuffers = model.IsUsingStaticBuffers();
    if (ImGui::Checkbox(Util::GUIDLabel("Using static buffers", model.GetGUID()).c_str(), &usingStaticBuffers))
        UpdateScheduler::Get().Add([&, usingStaticBuffers](){ model.UseStaticBuffers(usingStaticBuffers); });
    ImGui::SameLine();
    GeneralUI::HelpMarker("Applies to all meshes.");

    bool keepVertexData = model.GetNumVertices() != 0;
    if (ImGui::Checkbox(Util::GUIDLabel("Keep vertex data", model.GetGUID()).c_str(), &keepVertexData)) {
        if (keepVertexData)
            UpdateScheduler::Get().Add([&](){ model.RebuildFromBuffers(); });
        else
            UpdateScheduler::Get().Add([&](){ model.ClearGeometry(); });
    }
    ImGui::SameLine();
    GeneralUI::HelpMarker("Applies to all meshes.");

    ImGui::SeparatorText("Identifiers");
    AssetUI::Menu(model);
    if (ImGui::CollapsingHeader("World transform")) {
        GeneralUI::HelpMarker("Transformation will be applied to all instances of all submeshes of all meshes in this model.\n!CAUTION! meshes could be shared beteen models.");
        DirectX::XMFLOAT3X4 W = model.GetMeshes()[0]->GetSubmeshes()[0]->GetInstances()[0];
        if (MathUI::AffineTransformation(W))
            model.ApplyWorldTransform(W);
    }

    if (ImGui::CollapsingHeader("Armature")) {
        static std::uint32_t selectedBone = std::uint32_t(-1);
        BoneUI::Selector(selectedBone, model.GetBones());
        if (selectedBone != std::uint32_t(-1) && selectedBone < model.GetNumBones()) {
            ImGui::SeparatorText(model.GetBones()[selectedBone].GetName().c_str());
            BoneUI::Menu(model, selectedBone);
            ImGui::Separator();
        }
    }

    if (ImGui::CollapsingHeader(Util::GUIDLabel("Materials", "ModelMenu").c_str())) {
        MaterialUI::AdderPopupMenu(model, availableMaterials);
        ImGui::SameLine();
        MaterialUI::RemoverPopupMenu(model);

        MaterialUI::Menu(model.GetMaterials());
    }
}

void ModelUI::CreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(Util::GUIDLabel("+", "ModelCreatorPopupMenu").c_str()))
        ImGui::OpenPopup("ModelCreatorPopupMenu");
    if (ImGui::BeginPopup("ModelCreatorPopupMenu")) {
        Creator(batch);
        ImGui::EndPopup();
    }
}

