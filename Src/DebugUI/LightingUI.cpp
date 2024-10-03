#include "DebugUI/LightingUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/Util.h"
#include "DebugUI/AssetUI.h"

void LightingUI::DirectionalLightMenu(DirectionalLight& dirLight) {
    float direction[3];
    Util::StoreFloat3(dirLight.GetDirection(), direction);
    float diffuse[3];
    Util::StoreFloat3(dirLight.GetDiffuseColor(), diffuse);
    float specular[3];
    Util::StoreFloat3(dirLight.GetSpecularColor(), specular);

    AssetUI::Menu(dirLight);
    if (ImGui::DragFloat3("Direction", direction, .01f, -1.f, 1.f))
        Util::LoadFloat3(direction, dirLight.GetDirection());
    if (ImGui::ColorEdit3("Diffuse", diffuse))
        Util::LoadFloat3(diffuse, dirLight.GetDiffuseColor());
    if (ImGui::ColorEdit3("Specular", specular))
        Util::LoadFloat3(specular, dirLight.GetSpecularColor());
}

