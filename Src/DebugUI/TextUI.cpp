#include "DebugUI/TextUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/GeneralUI.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/Util.h"
#include "DebugUI/SpriteUI.h"

void TextUI::Creator(AssetBatch& batch) {
    static char filePath[128] = "";
    static std::string content = "";
    static char name[128] = "";
    static float origin[2] = { 0.f, 0.f };
    static float offset[2] = { 0.f, 0.f };
    static float scale[2] = { 1.f, 1.f };
    static float layer = 0.f;
    static float angle = 0.f;
    static float color[4] = { 0.f, 0.f, 0.f, 1.f };
    static bool visible = true;

    ImGui::InputText("Name##TextCreator", name, std::size(name));
    ImGui::Checkbox("Visible##TextCreator", &visible);
    GeneralUI::InputTextMultiline("Contents##TextCreator", &content);
    bool validFilePath = GeneralUI::InputFilePath("File path##TextCreator", filePath, std::size(filePath));
    GeneralUI::SameLineError(!validFilePath, "file not found");

    ImGui::SeparatorText("Position");
    ImGui::DragFloat2("Origin##TextCreator", origin);
    ImGui::DragFloat2("Offset##TextCreator", offset);
    ImGui::DragFloat2("Scale##TextCreator", scale);
    ImGui::DragFloat("Layer##TextCreator", &layer);
    ImGui::DragFloat("Angle##TextCreator", &angle);

    ImGui::SeparatorText("Color");
    ImGui::ColorEdit4("Color##TextCreator", color);

    if (ImGui::Button("Create new sprite##TextCreator") && validFilePath) {
        UpdateScheduler::Get().Add([&](){
            DirectX::XMFLOAT4 C = { color[0], color[1], color[2], color[3] };
            DirectX::XMFLOAT2 Or = { origin[0], origin[1] };
            DirectX::XMFLOAT2 Of = { offset[0], offset[1] };
            DirectX::XMFLOAT2 S = { scale[0], scale[1] };
            batch.Add(std::make_shared<Text>(
                        filePath,
                        content,
                        std::string(name),
                        Or, Of, S,
                        layer,
                        angle,
                        DirectX::XMLoadFloat4(&C),
                        visible));
        });
    }
}

void TextUI::Menu(Text& text) {
    static std::string content;
    SpriteUI::Menu(text);
    GeneralUI::InputTextMultiline("Content", &content);
    if (ImGui::Button(Util::GUIDLabel("Save content", text.GetGUID()).c_str()))
        text.SetContent(content);
}

void TextUI::Menu(const Texts& texts) {
    for (auto& textPair : texts) {
        if (ImGui::TreeNode(textPair.second->GetName().c_str())) {
            Menu(*textPair.second);
            ImGui::TreePop();
        }
    }
}

void TextUI::CreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(Util::GUIDLabel("+", "TextCreatorPopupMenu").c_str())) 
        ImGui::OpenPopup("TextCreatorPopupMenu");
    if (ImGui::BeginPopup("TextCreatorPopupMenu")) {
        Creator(batch);
        ImGui::EndPopup();
    }
}

