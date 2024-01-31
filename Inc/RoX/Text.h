#pragma once

#include <string>

// Positioning
//      -y
//      |
// -x --+-- +x
//      |
//      +y

struct Text {
    std::wstring FilePath = L"";

    std::string Content = "";

    bool Visible = true;

    float Angle = 0.0f; // In radians.
    float Layer = 0.0f;
    float Scale = 1.0f;

    float PositionX = 0.0f;
    float PositionY = 0.0f;

    float OriginOffsetX = 0.0f;
    float OriginOffsetY = 0.0f;

};
