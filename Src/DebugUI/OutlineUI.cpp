#include "DebugUI/OutlineUI.h"

#include <ImGui/imgui.h>
#include <SimpleMath.h>

#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/Util.h"
#include "DebugUI/MathUI.h"
#include "DebugUI/AssetUI.h"

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

    float center[3] = { b.Center.x, b.Center.y, b.Center.z };
    float extents[3] = { b.Extents.x, b.Extents.y, b.Extents.z };

    if (ImGui::DragFloat3(Util::GUIDLabel("Center", outline.GetGUID()).c_str(), center))
        b.Center = { center[0], center[1], center[2] };
    if (ImGui::DragFloat3(Util::GUIDLabel("Extents", outline.GetGUID()).c_str(), extents))
        b.Extents = { extents[0], extents[1], extents[2] };
}

void OutlineUI::BoundingFrustumMenu(BoundingBodyOutline<DirectX::BoundingFrustum>& outline) {
    DirectX::BoundingFrustum& b = outline.GetBoundingBody();

    float origin[3] = { b.Origin.x, b.Origin.y, b.Origin.z };

    DirectX::SimpleMath::Quaternion q = b.Orientation;
    DirectX::XMFLOAT3 r = q.ToEuler();
    float rotation[3] = { r.x, r.y, r.z };

    if (ImGui::DragFloat3(Util::GUIDLabel("Origin", outline.GetGUID()).c_str(), origin))
        b.Origin = { origin[0], origin[1], origin[2] };
    if (ImGui::DragFloat3(Util::GUIDLabel("Rotation", outline.GetGUID()).c_str(), rotation))
        DirectX::XMStoreFloat4(&b.Orientation, DirectX::XMQuaternionRotationRollPitchYaw(
                    DirectX::XMConvertToRadians(rotation[0]), 
                    DirectX::XMConvertToRadians(rotation[1]), 
                    DirectX::XMConvertToRadians(rotation[2])));
    ImGui::DragFloat(Util::GUIDLabel("Right slope", outline.GetGUID()).c_str(), &b.RightSlope);
    ImGui::DragFloat(Util::GUIDLabel("Left slope", outline.GetGUID()).c_str(), &b.LeftSlope);
    ImGui::DragFloat(Util::GUIDLabel("Top slope", outline.GetGUID()).c_str(), &b.TopSlope);
    ImGui::DragFloat(Util::GUIDLabel("Bottom slope", outline.GetGUID()).c_str(), &b.BottomSlope);
    ImGui::DragFloat(Util::GUIDLabel("Near", outline.GetGUID()).c_str(), &b.Near);
    ImGui::DragFloat(Util::GUIDLabel("Far", outline.GetGUID()).c_str(), &b.Far);
}

void OutlineUI::BoundingOrientedBoxMenu(BoundingBodyOutline<DirectX::BoundingOrientedBox>& outline) {
    DirectX::BoundingOrientedBox& b = outline.GetBoundingBody();

    float center[3] = { b.Center.x, b.Center.y, b.Center.z };
    float extents[3] = { b.Extents.x, b.Extents.y, b.Extents.z };
    DirectX::SimpleMath::Quaternion q = b.Orientation;
    DirectX::XMFLOAT3 r = q.ToEuler();
    float rotation[3] = { r.x, r.y, r.z };

    if (ImGui::DragFloat3(Util::GUIDLabel("Center", outline.GetGUID()).c_str(), center))
        b.Center = { center[0], center[1], center[2] };
    if (ImGui::DragFloat3(Util::GUIDLabel("Extents", outline.GetGUID()).c_str(), extents))
        b.Extents = { extents[0], extents[1], extents[2] };
    if (ImGui::DragFloat3(Util::GUIDLabel("Rotation", outline.GetGUID()).c_str(), rotation))
        DirectX::XMStoreFloat4(&b.Orientation, DirectX::XMQuaternionRotationRollPitchYaw(
                    DirectX::XMConvertToRadians(rotation[0]), 
                    DirectX::XMConvertToRadians(rotation[1]), 
                    DirectX::XMConvertToRadians(rotation[2])));
}

void OutlineUI::BoundingSphereMenu(BoundingBodyOutline<DirectX::BoundingSphere>& outline) {
    DirectX::BoundingSphere& b = outline.GetBoundingBody();

    float center[3] = { b.Center.x, b.Center.y, b.Center.z };

    if (ImGui::DragFloat3(Util::GUIDLabel("Center", outline.GetGUID()).c_str(), center))
        b.Center = { center[0], center[1], center[2] };
    ImGui::DragFloat(Util::GUIDLabel("Radius", outline.GetGUID()).c_str(), &b.Radius);
}

void OutlineUI::GridMenu(GridOutline& outline) {
    float xDivisions = outline.GetXDivisions();
    float yDivisions = outline.GetYDivisions();
    if (ImGui::DragScalar(Util::GUIDLabel("X divisions", outline.GetGUID()).c_str(), ImGuiDataType_U16, &xDivisions))
        outline.SetXDivisions(xDivisions);
    if (ImGui::DragScalar(Util::GUIDLabel("Y divisions", outline.GetGUID()).c_str(), ImGuiDataType_U16, &yDivisions))
        outline.SetYDivisions(yDivisions);

    float xAxis[3];
    Util::StoreFloat3(outline.GetXAxis(), xAxis);
    float yAxis[3];
    Util::StoreFloat3(outline.GetYAxis(), yAxis);
    float origin[3];
    Util::StoreFloat3(outline.GetOrigin(), origin);

    if (ImGui::DragFloat3(Util::GUIDLabel("X axis", outline.GetGUID()).c_str(), xAxis))
        Util::LoadFloat3(xAxis, outline.GetXAxis());
    if (ImGui::DragFloat3(Util::GUIDLabel("Y axis", outline.GetGUID()).c_str(), yAxis))
        Util::LoadFloat3(yAxis, outline.GetYAxis());
    if (ImGui::DragFloat3(Util::GUIDLabel("Origin", outline.GetGUID()).c_str(), origin))
        Util::LoadFloat3(origin, outline.GetOrigin());
}

void OutlineUI::RingMenu(RingOutline& outline) {
    float minorAxis[3];
    Util::StoreFloat3(outline.GetMinorAxis(), minorAxis);
    float majorAxis[3];
    Util::StoreFloat3(outline.GetMajorAxis(), majorAxis);
    float origin[3];
    Util::StoreFloat3(outline.GetOrigin(), origin);

    if (ImGui::DragFloat3(Util::GUIDLabel("Minor axis", outline.GetGUID()).c_str(), minorAxis))
        Util::LoadFloat3(minorAxis, outline.GetMinorAxis());
    if (ImGui::DragFloat3(Util::GUIDLabel("Major axis", outline.GetGUID()).c_str(), majorAxis))
        Util::LoadFloat3(majorAxis, outline.GetMajorAxis());
    if (ImGui::DragFloat3(Util::GUIDLabel("Origin", outline.GetGUID()).c_str(), origin))
        Util::LoadFloat3(origin, outline.GetOrigin());
}

void OutlineUI::RayMenu(RayOutline& outline) {
    bool normalized = outline.IsNormalized();
    if (ImGui::Checkbox(Util::GUIDLabel("Normalized", outline.GetGUID()).c_str(), &normalized))
        outline.SetNormalized(normalized);

    float direction[3];
    Util::StoreFloat3(outline.GetDirection(), direction);
    float origin[3];
    Util::StoreFloat3(outline.GetOrigin(), origin);

    if (ImGui::DragFloat3(Util::GUIDLabel("Direction", outline.GetGUID()).c_str(), direction))
        Util::LoadFloat3(direction, outline.GetDirection());
    if (ImGui::DragFloat3(Util::GUIDLabel("Origin##", outline.GetGUID()).c_str(), origin))
        Util::LoadFloat3(origin, outline.GetOrigin());
}

void OutlineUI::TriangleMenu(TriangleOutline& outline) {
    float pointA[3];
    Util::StoreFloat3(outline.GetPointA(), pointA);
    float pointB[3];
    Util::StoreFloat3(outline.GetPointB(), pointB);
    float pointC[3];
    Util::StoreFloat3(outline.GetPointC(), pointC);

    if (ImGui::DragFloat3(Util::GUIDLabel("Point A", outline.GetGUID()).c_str(), pointA))
        Util::LoadFloat3(pointA, outline.GetPointA());
    if (ImGui::DragFloat3(Util::GUIDLabel("Point B", outline.GetGUID()).c_str(), pointB))
        Util::LoadFloat3(pointB, outline.GetPointB());
    if (ImGui::DragFloat3(Util::GUIDLabel("Point C", outline.GetGUID()).c_str(), pointC))
        Util::LoadFloat3(pointC, outline.GetPointC());
}

void OutlineUI::QuadMenu(QuadOutline& outline) {
    float pointA[3];
    Util::StoreFloat3(outline.GetPointA(), pointA);
    float pointB[3];
    Util::StoreFloat3(outline.GetPointB(), pointB);
    float pointC[3];
    Util::StoreFloat3(outline.GetPointC(), pointC);
    float pointD[3];
    Util::StoreFloat3(outline.GetPointD(), pointD);

    if (ImGui::DragFloat3(Util::GUIDLabel("Point A", outline.GetGUID()).c_str(), pointA))
        Util::LoadFloat3(pointA, outline.GetPointA());
    if (ImGui::DragFloat3(Util::GUIDLabel("Point B", outline.GetGUID()).c_str(), pointB))
        Util::LoadFloat3(pointB, outline.GetPointB());
    if (ImGui::DragFloat3(Util::GUIDLabel("Point C", outline.GetGUID()).c_str(), pointC))
        Util::LoadFloat3(pointC, outline.GetPointC());
    if (ImGui::DragFloat3(Util::GUIDLabel("Point D", outline.GetGUID()).c_str(), pointD))
        Util::LoadFloat3(pointD, outline.GetPointD());
}

void OutlineUI::Menu(Outline& outline) {
    bool visible = outline.IsVisible();
    if (ImGui::Checkbox(Util::GUIDLabel("Visible", outline.GetGUID()).c_str(), &visible))
        outline.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    AssetUI::Menu(outline);

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

    float color[4];
    Util::StoreFloat4(outline.GetColor(), color);
    if (ImGui::CollapsingHeader("Color"))
        if (ImGui::ColorEdit4(Util::GUIDLabel("Color", outline.GetGUID()).c_str(), color))
            Util::LoadFloat4(color, outline.GetColor());
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

