#include "DebugUI/OutlineUI.h"

#include <ImGui/imgui.h>
#include <SimpleMath.h>

#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/Util.h"
#include "DebugUI/MathUI.h"
#include "DebugUI/IdentifiableUI.h"
#include "DebugUI/GeneralUI.h"

Outline::Type OutlineUI::TypeSelector() {
    static Outline::Type options[5] = { Outline::Type::Grid, Outline::Type::Ring, Outline::Type::Ray, Outline::Type::Triangle, Outline::Type::Quad };
    static std::string optionsStr[5] = { "Grid", "Ring", "Ray", "Triangle", "Quad" };

    static Outline::Type selected = options[0];
    static std::string selectedStr = optionsStr[0];

    if (ImGui::Button(Util::GUIDLabel(selectedStr, "OutlineTypeSelector").c_str()))
        ImGui::OpenPopup("OutlineTypeSelector");
    if (ImGui::BeginPopup("OutlineTypeSelector")) {
        for (std::uint8_t i = 0; i < std::size(options); ++i) {
            if (ImGui::Selectable(Util::GUIDLabel(optionsStr[i], "OutlineTypeSelector").c_str(), selected == options[i])) {
                selected = options[i];
                selectedStr = optionsStr[i];
            }
        }
        ImGui::EndPopup();
    }
    return selected;
}

void OutlineUI::Creator(AssetBatch& batch) {
    static char name[128] = "";
    static Outline::Type type = Outline::Type::Grid;
    static float color[4] = { 1.f, 1.f, 1.f, 1.f };
    static bool visible = true;

    ImGui::InputText("Name##OutlineCreator", name, std::size(name));
    type = TypeSelector();
    ImGui::ColorEdit4("Color##OutlineCreator", color);
    ImGui::Checkbox("Visible##OutlineCreator", &visible);

    if (ImGui::Button("Create new outline")) {
        UpdateScheduler::Get().Add([&](){ 
            std::shared_ptr<Outline> pOutline;
            DirectX::XMFLOAT4 C = { color[0], color[1], color[2], color[3] };
            switch (type) {
                case Outline::Type::Grid:
                    {
                        DirectX::XMFLOAT3 xAxis = { 1.f, 0.f, 0.f };
                        DirectX::XMFLOAT3 yAxis = { 0.f, 0.f, 1.f };
                        DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f };
                        pOutline = std::make_shared<GridOutline>(name, 2, 2, xAxis, yAxis, origin, DirectX::XMLoadFloat4(&C), visible);
                    }
                    break; 
                case Outline::Type::Ring:
                    {
                        DirectX::XMFLOAT3 majorAxis = { .5f, 0.f, 0.f };
                        DirectX::XMFLOAT3 minorAxis = { 0.f, 0.f, .5f };
                        DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f };
                        pOutline = std::make_shared<RingOutline>(name, majorAxis, minorAxis, origin, DirectX::XMLoadFloat4(&C), visible);
                    }
                    break; 
                case Outline::Type::Ray:
                    {
                        DirectX::XMFLOAT3 direction = { 1.f, 0.f, 0.f };
                        DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f };
                        pOutline = std::make_shared<RayOutline>(name, direction, origin, DirectX::XMLoadFloat4(&C), false, visible);
                    }
                    break; 
                case Outline::Type::Triangle:
                    {
                        DirectX::XMFLOAT3 pointA = { 0.f, 0.f, 0.f };
                        DirectX::XMFLOAT3 pointB = { 1.f, 0.f, 0.f };
                        DirectX::XMFLOAT3 pointC = { .5f, 1.f, 0.f };
                        pOutline = std::make_shared<TriangleOutline>(name, pointA, pointB, pointC, DirectX::XMLoadFloat4(&C), visible);
                    }
                    break; 
                default:
                    {
                        DirectX::XMFLOAT3 pointA = { 0.f, 0.f, 0.f };
                        DirectX::XMFLOAT3 pointB = { 0.f, 1.f, 0.f };
                        DirectX::XMFLOAT3 pointC = { 1.f, 0.f, 0.f };
                        DirectX::XMFLOAT3 pointD = { 1.f, 1.f, 0.f };
                        pOutline = std::make_shared<QuadOutline>(name, pointA, pointB, pointC, pointD, DirectX::XMLoadFloat4(&C), visible);
                    }
                    break; 
            }
            batch.Add(std::move(pOutline)); 
        });
    }
}

void OutlineUI::BoundingBoxMenu(BoundingBodyOutline<DirectX::BoundingBox>& outline) {
    DirectX::BoundingBox& b = outline.GetBoundingBody();

    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Center", outline.GetGUID()), b.Center, speed);
    MathUI::Vector(Util::GUIDLabel("Extents", outline.GetGUID()), b.Extents, speed);
}

void OutlineUI::BoundingFrustumMenu(BoundingBodyOutline<DirectX::BoundingFrustum>& outline) {
    DirectX::BoundingFrustum& b = outline.GetBoundingBody();

    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Origin", outline.GetGUID()), b.Origin, speed);
    MathUI::QuaternionWithEulerControlls(Util::GUIDLabel("Rotation (euler)", outline.GetGUID()), b.Orientation, speed);
    ImGui::DragFloat(Util::GUIDLabel("Right slope", outline.GetGUID()).c_str(), &b.RightSlope, speed);
    ImGui::DragFloat(Util::GUIDLabel("Left slope", outline.GetGUID()).c_str(), &b.LeftSlope, speed);
    ImGui::DragFloat(Util::GUIDLabel("Top slope", outline.GetGUID()).c_str(), &b.TopSlope, speed);
    ImGui::DragFloat(Util::GUIDLabel("Bottom slope", outline.GetGUID()).c_str(), &b.BottomSlope, speed);
    ImGui::DragFloat(Util::GUIDLabel("Near", outline.GetGUID()).c_str(), &b.Near, speed);
    ImGui::DragFloat(Util::GUIDLabel("Far", outline.GetGUID()).c_str(), &b.Far, speed);
}

void OutlineUI::BoundingOrientedBoxMenu(BoundingBodyOutline<DirectX::BoundingOrientedBox>& outline) {
    DirectX::BoundingOrientedBox& b = outline.GetBoundingBody();

    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Center", outline.GetGUID()), b.Center, speed);
    MathUI::Vector(Util::GUIDLabel("Extents", outline.GetGUID()), b.Extents, speed);
    MathUI::QuaternionWithEulerControlls(Util::GUIDLabel("Rotation (euler)", outline.GetGUID()), b.Orientation, speed);
}

void OutlineUI::BoundingSphereMenu(BoundingBodyOutline<DirectX::BoundingSphere>& outline) {
    DirectX::BoundingSphere& b = outline.GetBoundingBody();

    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Center", outline.GetGUID()), b.Center, speed);
    ImGui::DragFloat(Util::GUIDLabel("Radius", outline.GetGUID()).c_str(), &b.Radius, speed);
}

void OutlineUI::GridMenu(GridOutline& outline) {
    std::uint16_t xDivisions = outline.GetXDivisions();
    std::uint16_t yDivisions = outline.GetYDivisions();

    float speed = GeneralUI::DragSpeedControls();
    if (ImGui::DragScalar(Util::GUIDLabel("X divisions", outline.GetGUID()).c_str(), ImGuiDataType_U16, &xDivisions, speed))
        outline.SetXDivisions(xDivisions);
    if (ImGui::DragScalar(Util::GUIDLabel("Y divisions", outline.GetGUID()).c_str(), ImGuiDataType_U16, &yDivisions, speed))
        outline.SetYDivisions(yDivisions);
    MathUI::Vector(Util::GUIDLabel("X axis", outline.GetGUID()), outline.GetXAxis(), speed);
    MathUI::Vector(Util::GUIDLabel("Y axis", outline.GetGUID()), outline.GetYAxis(), speed);
    MathUI::Vector(Util::GUIDLabel("Origin", outline.GetGUID()), outline.GetOrigin(), speed);
}

void OutlineUI::RingMenu(RingOutline& outline) {
    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Minor axis", outline.GetGUID()), outline.GetMinorAxis(), speed);
    MathUI::Vector(Util::GUIDLabel("Major axis", outline.GetGUID()), outline.GetMajorAxis(), speed);
    MathUI::Vector(Util::GUIDLabel("Origin", outline.GetGUID()), outline.GetOrigin(), speed);
}

void OutlineUI::RayMenu(RayOutline& outline) {
    bool normalized = outline.IsNormalized();
    if (ImGui::Checkbox(Util::GUIDLabel("Normalized", outline.GetGUID()).c_str(), &normalized))
        outline.SetNormalized(normalized);
    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Direction", outline.GetGUID()), outline.GetDirection(), speed);
    MathUI::Vector(Util::GUIDLabel("Origin", outline.GetGUID()), outline.GetOrigin(), speed);
}

void OutlineUI::TriangleMenu(TriangleOutline& outline) {
    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Point A", outline.GetGUID()), outline.GetPointA(), speed);
    MathUI::Vector(Util::GUIDLabel("Point B", outline.GetGUID()), outline.GetPointB(), speed);
    MathUI::Vector(Util::GUIDLabel("Point C", outline.GetGUID()), outline.GetPointC(), speed);
}

void OutlineUI::QuadMenu(QuadOutline& outline) {
    float speed = GeneralUI::DragSpeedControls();
    MathUI::Vector(Util::GUIDLabel("Point A", outline.GetGUID()), outline.GetPointA(), speed);
    MathUI::Vector(Util::GUIDLabel("Point B", outline.GetGUID()), outline.GetPointB(), speed);
    MathUI::Vector(Util::GUIDLabel("Point C", outline.GetGUID()), outline.GetPointC(), speed);
    MathUI::Vector(Util::GUIDLabel("Point D", outline.GetGUID()), outline.GetPointD(), speed);
}

void OutlineUI::Menu(Outline& outline) {
    bool visible = outline.IsVisible();
    if (ImGui::Checkbox(Util::GUIDLabel("Visible", outline.GetGUID()).c_str(), &visible))
        outline.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    IdentifiableUI::Menu(outline);

    if (ImGui::CollapsingHeader("Position")) {
        switch (outline.GetType()) {
            case Outline::Type::BoundingBox: 
                BoundingBoxMenu(static_cast<BoundingBodyOutline<DirectX::BoundingBox>&>(outline));
                break;
            case Outline::Type::BoundingFrustum:
                BoundingFrustumMenu(static_cast<BoundingBodyOutline<DirectX::BoundingFrustum>&>(outline));
                break;
            case Outline::Type::BoundingOrientedBox:
                BoundingOrientedBoxMenu(static_cast<BoundingBodyOutline<DirectX::BoundingOrientedBox>&>(outline));
                break;
            case Outline::Type::BoundingSphere:
                BoundingSphereMenu(static_cast<BoundingBodyOutline<DirectX::BoundingSphere>&>(outline));
                break;
            case Outline::Type::Grid: 
                GridMenu(static_cast<GridOutline&>(outline));
                break;
            case Outline::Type::Ring:
                RingMenu(static_cast<RingOutline&>(outline));
                break;
            case Outline::Type::Ray:
                RayMenu(static_cast<RayOutline&>(outline));
                break;
            case Outline::Type::Triangle:
                TriangleMenu(static_cast<TriangleOutline&>(outline));
                break;
            case Outline::Type::Quad:
                QuadMenu(static_cast<QuadOutline&>(outline));
                break;
        }
    }

    if (ImGui::CollapsingHeader("Color")) {
        float speed = GeneralUI::DragSpeedControls();
        MathUI::ColorVector(Util::GUIDLabel("Color", outline.GetGUID()), outline.GetColor(), speed);
    }
}

void OutlineUI::Menu(const Outlines& outlines) {
    for (auto& outlinePair : outlines) {
        if (ImGui::TreeNode(outlinePair.second->GetName().c_str())) {
            Menu(*outlinePair.second);
            ImGui::TreePop();
        }
    }
}

void OutlineUI::CreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(Util::GUIDLabel("+", "AddOutlineToBatch").c_str())) 
        ImGui::OpenPopup("AddOutlineToBatch");
    if (ImGui::BeginPopup("AddOutlineToBatch")) {
        Creator(batch);
        ImGui::EndPopup();
    }
}

