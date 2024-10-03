#include "DebugUI/SpriteUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/Util.h"
#include "DebugUI/GeneralUI.h"
#include "DebugUI/IdentifiableUI.h"

void SpriteUI::Position(Sprite& sprite) {
    float origin[2] = { sprite.GetOrigin().x, sprite.GetOrigin().y };
    float offset[2] = { sprite.GetOffset().x, sprite.GetOffset().y };
    float scale[2] = { sprite.GetScale().x, sprite.GetScale().y };
    float layer = sprite.GetLayer();
    float angle = DirectX::XMConvertToDegrees(sprite.GetAngle());

    if (ImGui::DragFloat2(Util::GUIDLabel("Origin", sprite.GetGUID()).c_str(), origin))
        sprite.GetOrigin() = { origin[0], origin[1] };
    if (ImGui::DragFloat2(Util::GUIDLabel("Offset", sprite.GetGUID()).c_str(), offset))
        sprite.GetOffset() = { offset[0], offset[1] };
    if (ImGui::DragFloat2(Util::GUIDLabel("Scale", sprite.GetGUID()).c_str(), scale))
        sprite.GetScale() = { scale[0], scale[1] };
    if (ImGui::DragFloat(Util::GUIDLabel("Layer", sprite.GetGUID()).c_str(), &layer))
        sprite.SetLayer(layer);
    if (ImGui::DragFloat(Util::GUIDLabel("Angle", sprite.GetGUID()).c_str(), &angle))
        sprite.SetAngle(DirectX::XMConvertToRadians(angle));
}

void SpriteUI::Creator(AssetBatch& batch) {
    static char filePath[128] = "";
    static char name[128] = "";
    static float origin[2] = { 0.f, 0.f };
    static float offset[2] = { 0.f, 0.f };
    static float scale[2] = { 1.f, 1.f };
    static float layer = 0.f;
    static float angle = 0.f;
    static float color[4] = { 1.f, 1.f, 1.f, 1.f };
    static bool visible = true;

    ImGui::InputText("Name##SpriteCreator", name, std::size(name));
    ImGui::Checkbox("Visible##SpriteCreator", &visible);
    bool validFilePath = GeneralUI::InputFilePath("File path##SpriteCreator", filePath, std::size(filePath));
    GeneralUI::SameLineError(!validFilePath, "file not found");

    ImGui::SeparatorText("Position");
    ImGui::DragFloat2("Origin##SpriteCreator", origin);
    ImGui::DragFloat2("Offset##SpriteCreator", offset);
    ImGui::DragFloat2("Scale##SpriteCreator", scale);
    ImGui::DragFloat("Layer##SpriteCreator", &layer);
    ImGui::DragFloat("Angle##SpriteCreator", &angle);

    ImGui::SeparatorText("Color");
    ImGui::ColorEdit4("Color##SpriteCreator", color);

    if (ImGui::Button("Create new sprite##SpriteCreator") && validFilePath) {
        UpdateScheduler::Get().Add([&]() {
            DirectX::XMFLOAT4 C = { color[0], color[1], color[2], color[3] };
            DirectX::XMFLOAT2 Or = { origin[0], origin[1] };
            DirectX::XMFLOAT2 Of = { offset[0], offset[1] };
            DirectX::XMFLOAT2 S = { scale[0], scale[1] };
            batch.Add(std::make_shared<Sprite>(
                        filePath,
                        std::string(name),
                        Or, Of, S,
                        layer,
                        angle,
                        DirectX::XMLoadFloat4(&C),
                        visible));
        });
    }
}

void SpriteUI::Menu(Sprite& sprite) {
    bool visible = sprite.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        sprite.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    IdentifiableUI::Menu(sprite);

    ImGui::SeparatorText("Textures");
    ImGui::Text("File: %ws", sprite.GetFilePath().c_str());

    ImGui::SeparatorText("Position");
    Position(sprite);
    float color[4];
    Util::StoreFloat4(sprite.GetColor(), color);

    ImGui::SeparatorText("Color");
    if (ImGui::ColorEdit4(Util::GUIDLabel("Color", sprite.GetGUID()).c_str(), color))
        Util::LoadFloat4(color, sprite.GetColor());
}

void SpriteUI::Menu(const Sprites& sprites) {
    for (auto& spritePair : sprites) {
        if (ImGui::TreeNode(spritePair.second->GetName().c_str())) {
            Menu(*spritePair.second);
            ImGui::TreePop();
        }
    }
}

void SpriteUI::CreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(Util::GUIDLabel("+", "SpriteCreatorPopupMenu").c_str())) 
        ImGui::OpenPopup("SpriteCreatorPopupMenu");
    if (ImGui::BeginPopup("SpriteCreatorPopupMenu")) {
        Creator(batch);
        ImGui::EndPopup();
    }
}

