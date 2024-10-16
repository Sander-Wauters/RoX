#pragma once

#include <functional>
#include <string>

namespace GeneralUI {
    bool InputTextMultiline(const char* label, std::string* input);
    bool InputFilePath(const char* label, char* input, std::uint16_t inputSize);

    void HelpMarker(const char* desc);

    void ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove);

    void Error(bool show, const char* label);
    void SameLineError(bool show, const char* label);

    float DragSpeedControls();
}
