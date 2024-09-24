#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Outline.h>

namespace OutlineUI {
    Outline::Type TypeSelector();

    void Creator(AssetBatch& batch);

    void BoundingBoxMenu(BoundingBodyOutline<DirectX::BoundingBox>& outline);
    void BoundingFrustumMenu(BoundingBodyOutline<DirectX::BoundingFrustum>& outline);
    void BoundingOrientedBoxMenu(BoundingBodyOutline<DirectX::BoundingOrientedBox>& outline);
    void BoundingSphereMenu(BoundingBodyOutline<DirectX::BoundingSphere>& outline);
    void GridMenu(GridOutline& outline);
    void RingMenu(RingOutline& outline);
    void RayMenu(RayOutline& outline);
    void TriangleMenu(TriangleOutline& outline);
    void QuadMenu(QuadOutline& outline);

    void Menu(Outline& outline);
    void Menu(const Outlines& outlines);

    void CreatorPopupMenu(AssetBatch& batch);
}

