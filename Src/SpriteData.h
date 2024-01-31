#pragma once

#include "Util/pch.h"

struct SpriteData {   
    UINT DescriptorHeapIndex = -1;
    Microsoft::WRL::ComPtr<ID3D12Resource> pTexture = nullptr;

    RECT StretchRect = { 0, 0, 1, 1};

    DirectX::XMFLOAT2 Origin = { 0.0f, 0.0f };
    DirectX::FXMVECTOR Tint = DirectX::Colors::White;
};
