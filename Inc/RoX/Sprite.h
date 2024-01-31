#pragma once

#include <string>

struct Sprite {
    std::wstring FilePath = L"";

    bool Visible = true;

    float Angle = 0.0f; // In radians.
    float Layer = 0.0f;

    float PositionX = 0;
    float PositionY = 0;

    float OriginOffsetX = 0;
    float OriginOffsetY = 0;

    float Width = 0;
    float Height = 0;

    float WidthStretch = 0;
    float HeightStretch = 0;
};
