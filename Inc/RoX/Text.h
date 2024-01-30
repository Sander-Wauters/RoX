#pragma once

#include <string>

struct Text {
    std::wstring FilePath = L"";

    std::string Content = "";

    bool Visible = true;

    float Angle = 0.0f; // In radians.
    float Layer = 0.0f;
    float Scale = 1.0f;

    int PositionX = 0;
    int PositionY = 0;

    int OriginOffsetX = 0;
    int OriginOffsetY = 0;

};
