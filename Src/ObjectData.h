#pragma once

#include "../Lib/DirectXTK12/Inc/GeometricPrimitive.h"
#include "../Lib/DirectXTK12/Inc/Effects.h"
#include "../Lib/DirectXTK12/Inc/SpriteFont.h"

#include "Util/pch.h"

namespace ObjectData {
    struct Sprite {   
        UINT DescriptorHeapIndex = -1;
        Microsoft::WRL::ComPtr<ID3D12Resource> pTexture = nullptr;

        RECT StretchRect = { 0, 0, 1, 1};

        DirectX::XMFLOAT2 Origin = { 0.0f, 0.0f };
        DirectX::FXMVECTOR Tint = DirectX::Colors::White;
    };

    struct Text {
        UINT DescriptorHeapIndex = -1;
        std::unique_ptr<DirectX::SpriteFont> pSpriteFont = nullptr;
    };

    struct StaticGeometry {
        std::unique_ptr<DirectX::GeometricPrimitive> pGeometricPrimitive = nullptr;     
        std::unique_ptr<DirectX::NormalMapEffect> pEffect = nullptr;
    };

    struct Texture {
        UINT DescriptorHeapIndex = -1;
        Microsoft::WRL::ComPtr<ID3D12Resource> pTexture = nullptr;
    };
}
