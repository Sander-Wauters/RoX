#pragma once

#include <string>

struct Text {
    std::wstring FilePath = L"";

    std::string Content = "";

    bool Visible = true;

    float Angle = 0.0f; // In radians.
    float Layer = 0.0f;
    float Scale = 1.0f;

    float PositionX = 0;
    float PositionY = 0;

    float OriginOffsetX = 0;
    float OriginOffsetY = 0;

};
