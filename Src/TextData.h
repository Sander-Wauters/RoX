#pragma once

#include "../Lib/DirectXTK12/Inc/SpriteFont.h"
#include <memory>

struct TextData {
    UINT DescriptorHeapIndex = -1;
    std::unique_ptr<DirectX::SpriteFont> pSpriteFont = nullptr;
};
