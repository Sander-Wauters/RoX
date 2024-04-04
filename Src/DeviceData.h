#pragma once

#include <GeometricPrimitive.h>
#include <Effects.h>
#include <SpriteFont.h>

#include "Util/pch.h"

namespace DeviceData {

    struct Mesh {
        std::unique_ptr<DirectX::GeometricPrimitive> pGeometricPrimitive = nullptr;     
    };

    struct Material {
        std::unique_ptr<DirectX::NormalMapEffect> pEffect = nullptr;
    };

    struct Texture {
        UINT DescriptorHeapIndex = -1;
        Microsoft::WRL::ComPtr<ID3D12Resource> pTexture = nullptr;
    };

    struct Text {
        UINT DescriptorHeapIndex = -1;
        std::unique_ptr<DirectX::SpriteFont> pSpriteFont = nullptr;
    };

}
