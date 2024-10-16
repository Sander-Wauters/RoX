#include "DebugUI/LightingUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/Util.h"
#include "DebugUI/IdentifiableUI.h"
#include "DebugUI/GeneralUI.h"
#include "DebugUI/MathUI.h"

void LightingUI::DirectionalLightMenu(DirectionalLight& dirLight) {
    IdentifiableUI::Menu(dirLight);
    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Direction", dirLight.GetGUID()), dirLight.GetDirection(), speed);
    MathUI::ColorVector(Util::GUIDLabel("Diffuse", dirLight.GetGUID()), dirLight.GetDiffuseColor(), speed);
    MathUI::ColorVector(Util::GUIDLabel("Specular", dirLight.GetGUID()), dirLight.GetSpecularColor(), speed);
}

