#include "DebugUI/GeneralUI.h"

#include <fstream>

#include <ImGui/imgui.h>

constexpr ImVec4 COLOR_ERROR = { 255.f, 0.f, 0.f, 1.f };

struct InputTextCallback_UserData {
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

int InputTextCallback(ImGuiInputTextCallbackData* data) {
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback) {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputTextMultilineImpl(const char* label, std::string* input, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = input;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputTextMultiline(label, (char*)input->c_str(), input->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool GeneralUI::InputTextMultiline(const char* label, std::string* input) {
    return InputTextMultilineImpl(label, input);
}

bool GeneralUI::InputFilePath(const char* label, char* input, std::uint16_t inputSize) {
    std::ifstream fin(input);
    if (ImGui::InputText(label, input, inputSize))
        fin = std::ifstream(input);
    return fin.good();
}

void GeneralUI::HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void GeneralUI::ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove) {
    static ImU32 steps = 1;

    if (ImGui::Button("Add"))
        onAdd();
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    ImGui::InputScalar(label, ImGuiDataType_U32, pIndex, &steps);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Remove"))
        onRemove();
}

void GeneralUI::Error(bool show, const char* label) {
    if (show)
        ImGui::TextColored(COLOR_ERROR, "%s", label);
}

void GeneralUI::SameLineError(bool show, const char* label) {
    if (show) {
        ImGui::SameLine();
        ImGui::TextColored(COLOR_ERROR, "%s", label);
    }
}

float GeneralUI::DragSpeedControls() {
    static float speed = .25f;
    ImGui::DragFloat("Drag speed", &speed, 0.001f);
    ImGui::Separator();
    return speed;
}

