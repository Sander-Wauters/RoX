#include "DebugUI/CameraUI.h"

#include <limits>
#include <algorithm>

#undef max
#include <ImGui/imgui.h>

void CameraUI::Menu(Camera& camera) {
    ImGuiViewport* pViewport = ImGui::GetMainViewport();
    static float z[2] = { camera.GetNearZ(), camera.GetFarZ() };
    static float windowSize[2] = { pViewport->WorkSize.x, pViewport->WorkSize.y };
    static float fovY = camera.GetFovY();

    if (ImGui::Button("Reset")) {
        z[0] = .1f; z[1] = 2000.f;
        windowSize[0] = pViewport->WorkSize.x; windowSize[1] = pViewport->WorkSize.y;
        fovY = DirectX::XM_PIDIV4;
    }

    ImGui::SameLine();

    bool isOrthographic = camera.IsOrthographic();
    ImGui::Checkbox("Orthographic", &isOrthographic);
    
    ImGui::SeparatorText("Projection");
    ImGui::DragFloat2("Z planes", z, 1.f, .01f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Text("Fov x: %f", camera.GetFovX());
    ImGui::DragFloat("Fov y", &fovY, DirectX::XMConvertToRadians(.5f), .01f, std::numeric_limits<float>::max());

    ImGui::SeparatorText("Window size");
    ImGui::Text("Aspect: %f", camera.GetAspect());
    ImGui::DragFloat2("Window size", windowSize, 1.f, .1f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Text("Near plane width:  %f", camera.GetNearWindowWidth());
    ImGui::Text("Near plane height: %f", camera.GetNearWindowHeight());
    ImGui::Spacing();
    ImGui::Text("Far plane width:   %f", camera.GetFarWindowWidth());
    ImGui::Text("Far plane height:  %f", camera.GetFarWindowHeight());

    if (isOrthographic)
        camera.SetOrthographicView(windowSize[0], windowSize[1], z[0], std::max(z[1], 1.f));
    else
        camera.SetPerspectiveView(fovY, windowSize[0] / windowSize[1], z[0], std::max(z[1], 1.f));
    camera.Update();
}

