#pragma once

#include "Util/pch.h"

struct SpriteData {   
    Microsoft::WRL::ComPtr<ID3D12Resource> pTexture = nullptr;

    RECT TileRect = { 0, 0, 1, 1 };
    RECT StretchRect = { 0, 0, 1, 1};

    DirectX::XMFLOAT2 Origin = { 0.0f, 0.0f };
    DirectX::FXMVECTOR Tint = DirectX::Colors::White;
};
