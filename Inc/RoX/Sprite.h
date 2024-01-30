#pragma once

#include <string>

struct Sprite {
    std::wstring FilePath = L"";

    bool Visible = true;

    float Angle = 0.0f; // In radians.
    float Layer = 0.0f;

    int PositionX = 0;
    int PositionY = 0;

    int OriginOffsetX = 0;
    int OriginOffsetY = 0;

    int Width = 0;
    int Height = 0;

    int WidthStretch = 0;
    int HeightStretch = 0;
};
