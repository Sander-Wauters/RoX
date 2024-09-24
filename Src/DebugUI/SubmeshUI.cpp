#include "DebugUI/SubmeshUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/GeneralUI.h"
#include "DebugUI/Util.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/MaterialUI.h"
#include "DebugUI/MathUI.h"
#include "DebugUI/AssetUI.h"

void SubmeshUI::Instances(Submesh& submesh) {
    static std::uint32_t index = 0;
    if (index < 0)
        index = 0;
    if (index >= submesh.GetNumInstances())
        index = submesh.GetNumInstances() - 1;

    static ImU32 steps = 1;

    std::uint32_t numCulled = submesh.GetNumCulled();

    ImGui::Text("Total instances:   %d", submesh.GetNumInstances());
    ImGui::SameLine();
    GeneralUI::HelpMarker("If instancing isn't enabled on the material then the first instance will be used.");
    ImGui::Text("Visible instances: %d", submesh.GetNumVisibleInstances());
    ImGui::Text("Culled instances:  %d", submesh.GetNumCulled());
    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    if (ImGui::InputScalar("Cull", ImGuiDataType_U32, &numCulled, &steps)) {
        if (numCulled <= submesh.GetNumInstances())
            submesh.SetNumberCulled(numCulled);
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    GeneralUI::HelpMarker("The last n instances will be culled.");

    ImGui::Separator();
    std::function<void()> onAdd = [&]() {
        submesh.GetInstances().push_back({});
        DirectX::XMStoreFloat3x4(&submesh.GetInstances().back(), DirectX::XMMatrixIdentity());
    };
    std::function<void()> onRemove = [&]() {
        if (index >= 0 && index < submesh.GetNumInstances() && submesh.GetNumInstances() >= 2) {
            std::swap(submesh.GetInstances()[index], submesh.GetInstances().back());
            submesh.GetInstances().pop_back();
        }
    };
    GeneralUI::ArrayControls("Index##SubmeshInstances", &index, onAdd, onRemove);

    ImGui::Spacing();

    if (index >= 0 && index < submesh.GetNumInstances()) {
        MathUI::AffineTransformation(submesh.GetInstances()[index]);
        ImGui::Separator();
        MathUI::Matrix(submesh.GetInstances()[index]);
    }
}

void SubmeshUI::VertexIndexing(Submesh& submesh) {
    static ImU32 steps = 1;

    std::uint32_t indexCount = submesh.GetIndexCount();
    std::uint32_t startIndex = submesh.GetStartIndex();
    std::uint32_t vertexOffset = submesh.GetVertexOffset();

    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    if (ImGui::InputScalar("Index count", ImGuiDataType_U32, &indexCount, &steps))
        submesh.SetIndexCount(indexCount);
    if (ImGui::InputScalar("Start index", ImGuiDataType_U32, &startIndex, &steps))
        submesh.SetStartIndex(startIndex);
    if (ImGui::InputScalar("Vertex offset", ImGuiDataType_U32, &vertexOffset, &steps))
        submesh.SetVertexOffset(vertexOffset);
    ImGui::PopItemWidth();
}

void SubmeshUI::Selector(std::uint32_t& index, const std::vector<std::unique_ptr<Submesh>>& submeshes) {
    for (std::uint32_t i = 0; i < submeshes.size(); ++i) {
        if (ImGui::Selectable(Util::GUIDLabel(submeshes[i]->GetName(), submeshes[i]->GetGUID()).c_str(), index == i, ImGuiSelectableFlags_DontClosePopups))
            index = i;
    }
}

void SubmeshUI::Creator(IMesh& iMesh, std::vector<std::shared_ptr<Material>>& availableMaterials) {
    static char name[128] = "";
    static std::uint32_t materialIndex = 0;
    static bool visible = true;

    ImGui::InputText("Name##SubmeshCreator", name, std::size(name));
    ImGui::Checkbox("Visible##SubmeshCreator", &visible);

    ImGui::SeparatorText("Available materials");
    MaterialUI::Selector(materialIndex, availableMaterials);
    if (ImGui::Button("Create new submesh##SubmeshCreator"))
        UpdateScheduler::Get().Add([&](){ iMesh.Add(std::make_unique<Submesh>(name, materialIndex, visible)); });
}

void SubmeshUI::Remover(IMesh& iMesh) {
    std::uint32_t selected = std::uint32_t(-1);
    Selector(selected, iMesh.GetSubmeshes());
    if (selected != std::uint32_t(-1))
        UpdateScheduler::Get().Add([&, selected](){ iMesh.RemoveSubmesh(selected); });
}

void SubmeshUI::Menu(Submesh& submesh, std::vector<std::shared_ptr<Material>>& availableMaterials) {
    bool visible = submesh.IsVisible();
    if (ImGui::Checkbox(Util::GUIDLabel("Visible", submesh.GetGUID()).c_str(), &visible))
        submesh.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    AssetUI::Menu(submesh);

    if (ImGui::CollapsingHeader("Available materials")) {
        std::uint32_t materialIndex = submesh.GetMaterialIndex();
        MaterialUI::Selector(materialIndex, availableMaterials);
        submesh.SetMaterialIndex(materialIndex);
    }

    if (ImGui::CollapsingHeader("Instancing"))
        Instances(submesh);

    if (ImGui::CollapsingHeader("Vertex indexing"))
        VertexIndexing(submesh);
}

void SubmeshUI::RemoverPopupMenu(IMesh& iMesh) {
    if (ImGui::Button(Util::GUIDLabel("-", "SubmeshRemoverPopupMenu").c_str())) 
        ImGui::OpenPopup("SubmeshRemoverPopupMenu");
    if (ImGui::BeginPopup("SubmeshRemoverPopupMenu")) {
        ImGui::SeparatorText("Remove submesh");
        Remover(iMesh);
        ImGui::EndPopup();
    }
}

