#include "RoX/DebugUI.h"

#include "RoX/AssetIO.h"
#include "RoX/MeshFactory.h"

#include "Util/pch.h"
#include "Util/dxtk12pch.h"

#undef min
#undef max
#include <ImGui/imgui.h>

constexpr ImVec4 COLOR_ERROR = { 255.f, 0.f, 0.f, 1.f };

// ---------------------------------------------------------------- //
//                          XMFLOAT util.
// ---------------------------------------------------------------- //

void DebugUI::StoreFloat2(DirectX::XMFLOAT2& in, float* out) {
    out[0] = in.x;
    out[1] = in.y;
}

void DebugUI::StoreFloat3(DirectX::XMFLOAT3& in, float* out) {
    out[0] = in.x;
    out[1] = in.y;
    out[2] = in.z;
}

void DebugUI::StoreFloat4(DirectX::XMFLOAT4& in, float* out) {
    out[0] = in.x;
    out[1] = in.y;
    out[2] = in.z;
    out[3] = in.w;
}

void DebugUI::LoadFloat2(float* in, DirectX::XMFLOAT2& out) {
    out.x = in[0];
    out.y = in[1];
}

void DebugUI::LoadFloat3(float* in, DirectX::XMFLOAT3& out) {
    out.x = in[0];
    out.y = in[1];
    out.z = in[2];
}

void DebugUI::LoadFloat4(float* in, DirectX::XMFLOAT4& out) {
    out.x = in[0];
    out.y = in[1];
    out.z = in[2];
    out.w = in[3];
}

// ---------------------------------------------------------------- //
//                          General helpers.
// ---------------------------------------------------------------- //

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

bool DebugUI::InputTextMultiline(const char* label, std::string* input) {
    return InputTextMultilineImpl(label, input);
}

bool DebugUI::InputFilePath(const char* label, char* input, std::uint16_t inputSize) {
    std::ifstream fin(input);
    if (ImGui::InputText(label, input, inputSize))
        fin = std::ifstream(input);
    return fin.good();
}

void DebugUI::HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

std::string DebugUI::GUIDLabel(std::string label, std::uint64_t GUID) {
    return label + "##" + std::to_string(GUID);
}

std::string DebugUI::GUIDLabel(std::string label, std::string GUID) {
    return label + "##" + GUID;
}

void DebugUI::ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove) {
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

void DebugUI::Error(bool show, const char* label) {
    if (show)
        ImGui::TextColored(COLOR_ERROR, "%s", label);
}

void DebugUI::SameLineError(bool show, const char* label) {
    if (show) {
        ImGui::SameLine();
        ImGui::TextColored(COLOR_ERROR, "%s", label);
    }
}

// ---------------------------------------------------------------- //
//                          Vertices and indices.
// ---------------------------------------------------------------- //

void DebugUI::Vertex(VertexPositionNormalTexture& vertex) {
    float position[3];
    StoreFloat3(vertex.position, position);
    float normal[3];
    StoreFloat3(vertex.normal, normal);
    float texture[2];
    StoreFloat2(vertex.textureCoordinate, texture);

    if (ImGui::DragFloat3("position", position))
        LoadFloat3(position, vertex.position);
    if (ImGui::DragFloat3("normal", normal))
        LoadFloat3(normal, vertex.normal);
    if (ImGui::DragFloat2("texture", texture))
        LoadFloat2(texture, vertex.textureCoordinate);
}

void DebugUI::Vertex(VertexPositionNormalTextureSkinning& vertex) {
    float position[3];
    StoreFloat3(vertex.position, position);
    float normal[3];
    StoreFloat3(vertex.normal, normal);
    float texture[2];
    StoreFloat2(vertex.textureCoordinate, texture);
    float weights[4];
    StoreFloat4(vertex.weights, weights);

    std::uint32_t indices[4] = { vertex.indices & 0xff, (vertex.indices >> 8) & 0xff, (vertex.indices >> 16) & 0xff, (vertex.indices >> 24) & 0xff };

    if (ImGui::DragFloat3("position", position))
        LoadFloat3(position, vertex.position);
    if (ImGui::DragFloat3("normal", normal))
        LoadFloat3(normal, vertex.normal);
    if (ImGui::DragFloat2("texture", texture))
        LoadFloat2(texture, vertex.textureCoordinate);
    if (ImGui::DragScalarN("blend indices", ImGuiDataType_U32, indices, 4))
        vertex.SetBlendIndices({ indices[0], indices[1], indices[2], indices[3] });
    if (ImGui::DragFloat4("blend weights", weights, .25f, 0.f, 0.f, "%.5f"))
        vertex.SetBlendWeights({ weights[0], weights[1], weights[2], weights[3] });
}

void DebugUI::Vertices(std::vector<VertexPositionNormalTexture>& vertices) {
    static std::uint32_t index = 0;
    if (index < 0) 
        index = 0;
    if (index >= vertices.size()) 
        index = vertices.size() - 1;

    std::function<void()> onAdd = [&]() {
        vertices.push_back({});
    };
    std::function<void()> onRemove = [&]() {
        if (index >= 0 && index < vertices.size() && vertices.size() >= 2) {
            std::swap(vertices[index], vertices.back());
            vertices.pop_back();
        }
    };
    ImGui::Text("Vertices: %lu", (unsigned long)vertices.size());

    ImGui::Separator();
    ArrayControls("index##Vertices", &index, onAdd, onRemove);
    if (index >= 0 && index < vertices.size())
        Vertex(vertices[index]);
}

void DebugUI::Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices) {
    static std::uint32_t index = 0;
    if (index < 0) 
        index = 0;
    if (index >= vertices.size()) 
        index = vertices.size() - 1;

    std::function<void()> onAdd = [&]() {
        vertices.push_back({});
    };
    std::function<void()> onRemove = [&]() {
        if (index >= 0 && index < vertices.size() && vertices.size() >= 2) {
            std::swap(vertices[index], vertices.back());
            vertices.pop_back();
        }
    };
    ImGui::Text("Vertices: %lu", (unsigned long)vertices.size());

    ImGui::Separator();

    ArrayControls("index##Vertices", &index, onAdd, onRemove);
    if (index >= 0 && index < vertices.size())
        Vertex(vertices[index]);
}

void DebugUI::Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices) {
    static std::uint32_t index = 0;
    if (index < 0)
        index = 0;
    if (index >= indices.size())
        index = indices.size() - 1;

    std::function<void()> onAdd = [&]() {
        indices.push_back(indices.size());
    };
    std::function<void()> onRemove = [&]() {
        if (index >= 0 && index < indices.size() && indices.size() >= 2) {
            std::swap(indices[index], indices.back());
            indices.pop_back();
        }
    };
    ImGui::Text("Indices: %lu", (unsigned long)indices.size());
    ImGui::Text("Vertices: %d", numVertices);

    ImGui::Separator();
    ArrayControls("index##Indices", &index, onAdd, onRemove);
    if (index >= 0 && index < indices.size()) {
        std::uint16_t selectedIndex = indices[index];
        if (selectedIndex < 0)
            selectedIndex = 0;
        if (selectedIndex >= numVertices)
            selectedIndex = numVertices - 1;
        if (ImGui::DragScalar("vertex index", ImGuiDataType_U16, &selectedIndex)) {
            if (selectedIndex >= 0 && index < numVertices) 
                indices[index] = selectedIndex;
        }
    }
}

// ---------------------------------------------------------------- //
//                          Matrices.
// ---------------------------------------------------------------- //

void DebugUI::Matrix(DirectX::XMMATRIX& matrix) {
    DirectX::XMFLOAT4X4 M;
    DirectX::XMStoreFloat4x4(&M, matrix);
    Matrix(M);
    matrix = DirectX::XMLoadFloat4x4(&M);
}

void DebugUI::Matrix(DirectX::XMFLOAT4X4& matrix) {
    ImGui::DragFloat4("r0", matrix.m[0], 0.25f);
    ImGui::SameLine();
    HelpMarker("matrix row-major");
    ImGui::DragFloat4("r1", matrix.m[1], 0.25f);
    ImGui::DragFloat4("r2", matrix.m[2], 0.25f);
    ImGui::DragFloat4("r3", matrix.m[3], 0.25f);
}

void DebugUI::Matrix(DirectX::XMFLOAT3X4& matrix) {
    static float r3[4] = { 0.f, 0.f, 0.f, 1.f };
    ImGui::DragFloat4("c0", matrix.m[0], 0.25f);
    ImGui::SameLine();
    HelpMarker("matrix column-major");
    ImGui::DragFloat4("c1", matrix.m[1], 0.25f);
    ImGui::DragFloat4("c2", matrix.m[2], 0.25f);
    ImGui::InputFloat4("c3", r3, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

bool DebugUI::AffineTransformation(DirectX::XMMATRIX& matrix) {
    static float origin[3] = { 0.f, 0.f, 0.f};
    static float translation[3] = { 0.f, 0.f, 0.f };
    static float scale[3] = { 1.f, 1.f, 1.f };
    static float rotation[3] = { 0.f, 0.f, 0.f };

    if (ImGui::Button("Apply")) {
        DirectX::XMVECTOR o = DirectX::XMVectorSet(origin[0], origin[1], origin[2], 0.f);
        DirectX::XMVECTOR t = DirectX::XMVectorSet(translation[0], translation[1], translation[2], 0.f);
        DirectX::XMVECTOR s = DirectX::XMVectorSet(scale[0], scale[1], scale[2], 0.f);
        DirectX::XMVECTOR r = DirectX::XMQuaternionRotationRollPitchYaw(
                DirectX::XMConvertToRadians(rotation[0]), 
                DirectX::XMConvertToRadians(rotation[1]), 
                DirectX::XMConvertToRadians(rotation[2]));
        matrix = DirectX::XMMatrixAffineTransformation(s, o, r, t);
        return true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        DirectX::XMVECTOR t;
        DirectX::XMVECTOR s;
        DirectX::XMVECTOR r;
        DirectX::XMMatrixDecompose(&s, &r, &t, matrix);

        DirectX::SimpleMath::Quaternion q(r);
        DirectX::XMFLOAT3 euler = q.ToEuler();
        r = DirectX::XMLoadFloat3(&euler);

        DirectX::XMFLOAT3 tf;
        DirectX::XMFLOAT3 sf;
        DirectX::XMFLOAT3 rf;
        DirectX::XMStoreFloat3(&tf, t);
        DirectX::XMStoreFloat3(&sf, s);
        DirectX::XMStoreFloat3(&rf, r);

        origin[0]      = 0.f;  origin[1]      = 0.f;  origin[2]      = 0.f;
        translation[0] = tf.x; translation[1] = tf.y; translation[2] = tf.z;
        scale[0]       = sf.x; scale[1]       = sf.y; scale[2]       = sf.z;
        rotation[0]    = rf.x; rotation[1]    = rf.y; rotation[2]    = rf.z;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        origin[0]      = 0.f; origin[1]      = 0.f; origin[2]      = 0.f;
        translation[0] = 0.f; translation[1] = 0.f; translation[2] = 0.f;
        scale[0]       = 1.f; scale[1]       = 1.f; scale[2]       = 1.f;
        rotation[0]    = 0.f; rotation[1]    = 0.f; rotation[2]    = 0.f;
    }

    ImGui::DragFloat3("origin", origin);
    ImGui::DragFloat3("translation", translation);
    ImGui::DragFloat3("scale", scale);
    ImGui::DragFloat3("rotation", rotation);

    return false;
}

bool DebugUI::AffineTransformation(DirectX::XMFLOAT4X4& matrix) {
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&matrix);
    bool transformatonApplied = AffineTransformation(M);
    DirectX::XMStoreFloat4x4(&matrix, M);
    return transformatonApplied;
}

bool DebugUI::AffineTransformation(DirectX::XMFLOAT3X4& matrix) {
    DirectX::XMMATRIX M = DirectX::XMLoadFloat3x4(&matrix);
    bool transformatonApplied = AffineTransformation(M);
    DirectX::XMStoreFloat3x4(&matrix, M);
    return transformatonApplied;
}

// ---------------------------------------------------------------- //
//                          Assets.
// ---------------------------------------------------------------- //

void DebugUI::AssetRemover(AssetBatch::AssetType type, AssetBatch& batch) {
    static std::uint64_t GUID = 0;
    static bool GUIDnotFound = false;

    static char name[128] = "";
    static bool nameNotFound = false;

    try {
        ImGui::InputScalar(GUIDLabel("GUID", "MaterialRemover").c_str(), ImGuiDataType_U64, &GUID);
        ImGui::SameLine();
        if (ImGui::SmallButton(GUIDLabel("Remove", "MaterialRemover_GUID").c_str())) {
            GUIDnotFound = false;
            batch.Remove(type, GUID);
        }
        Error(GUIDnotFound, "Material not found."); 
    } catch (std::out_of_range ex) {
        GUIDnotFound = true;
    }

    try {
        ImGui::InputText(GUIDLabel("Name", "MaterialRemover").c_str(), name, std::size(name));
        ImGui::SameLine();
        if (ImGui::SmallButton(GUIDLabel("Remove", "MaterialRemover_Name").c_str())) {
            nameNotFound = false;
            batch.Remove(type, name);
        }
        Error(nameNotFound, "Material not found."); 
    } catch (std::out_of_range ex) {
        nameNotFound = true;
    }
}

void DebugUI::AssetMenu(Asset& asset) {
    static char name[128] = "";
    static bool editName = false;

    ImGui::Text("GUID: %llu", asset.GetGUID());

    ImGui::Text("Name: %s", asset.GetName().c_str());
    ImGui::SameLine();
    if (ImGui::SmallButton(GUIDLabel("Change", asset.GetGUID()).c_str()))
        editName = !editName;

    if (editName) {
        ImGui::InputText(GUIDLabel("", asset.GetGUID()).c_str(), name, std::size(name));
        ImGui::SameLine();
        if (ImGui::Button(GUIDLabel("Save", asset.GetGUID()).c_str())) {
            asset.SetName(name);
            editName = false;
        }
    }
}

void DebugUI::AssetRemoverPopupMenu(AssetBatch::AssetType type, AssetBatch& batch) {
    std::string typeStr;
    switch (type) {
        case AssetBatch::AssetType::Material:
            typeStr = "Material";
            break;
        case AssetBatch::AssetType::Model:
            typeStr = "Model";
            break;
        case AssetBatch::AssetType::Sprite:
            typeStr = "Sprite";
            break;
        case AssetBatch::AssetType::Text:
            typeStr = "Text";
            break;
        case AssetBatch::AssetType::Outline:
            typeStr = "Outline";
            break;
    }

    if (ImGui::Button(GUIDLabel("-", "AssetRemoverPopupMenu" + typeStr).c_str())) 
        ImGui::OpenPopup(std::string("AssetRemoverPopupMenu" + typeStr).c_str());
    if (ImGui::BeginPopup(std::string("AssetRemoverPopupMenu" + typeStr).c_str())) {
        AssetRemover(type, batch);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Materials.
// ---------------------------------------------------------------- //

void DebugUI::RenderFlagsPresets(std::uint32_t& renderFlags) {
    if (ImGui::Selectable("None", renderFlags == RenderFlags::None, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::None;
    if (ImGui::Selectable("Default", renderFlags == RenderFlags::Default, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::Default;
    if (ImGui::Selectable("Skinned", renderFlags == RenderFlags::Skinned, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::Skinned;
    if (ImGui::Selectable("Wireframe", renderFlags == RenderFlags::Wireframe, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::Wireframe;
    if (ImGui::Selectable("Instanced", renderFlags == RenderFlags::Instanced, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::Instanced;
    if (ImGui::Selectable("WireframeInstanced", renderFlags == RenderFlags::WireframeInstanced, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::WireframeInstanced;
    if (ImGui::Selectable("WireframeSkinned", renderFlags == RenderFlags::WireframeSkinned, ImGuiSelectableFlags_DontClosePopups))
        renderFlags = RenderFlags::WireframeSkinned;
}

void DebugUI::RenderFlagsBlendState(std::uint32_t& renderFlags) {
    if (ImGui::Selectable("Opaque", renderFlags & RenderFlags::BlendState::Opaque, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::BlendState::Reset;
        renderFlags |= RenderFlags::BlendState::Opaque;
    }
    if (ImGui::Selectable("AlphaBlend", renderFlags & RenderFlags::BlendState::AlphaBlend, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::BlendState::Reset;
        renderFlags |= RenderFlags::BlendState::AlphaBlend;
    }
    if (ImGui::Selectable("Additive", renderFlags & RenderFlags::BlendState::Additive, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::BlendState::Reset;
        renderFlags |= RenderFlags::BlendState::Additive;
    }
    if (ImGui::Selectable("NonPremultiplied", renderFlags & RenderFlags::BlendState::NonPremultiplied, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::BlendState::Reset;
        renderFlags |= RenderFlags::BlendState::NonPremultiplied;
    }
    if (ImGui::Selectable("Reset", false, ImGuiSelectableFlags_DontClosePopups))
        renderFlags &= RenderFlags::BlendState::Reset;
}

void DebugUI::RenderFlagsDepthStencilState(std::uint32_t& renderFlags) {
    if (ImGui::Selectable("None", renderFlags & RenderFlags::DepthStencilState::None, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::DepthStencilState::Reset;
        renderFlags |= RenderFlags::DepthStencilState::None;
    }
    if (ImGui::Selectable("Default", renderFlags & RenderFlags::DepthStencilState::Default, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::DepthStencilState::Reset;
        renderFlags |= RenderFlags::DepthStencilState::Default;
    }
    if (ImGui::Selectable("Read", renderFlags & RenderFlags::DepthStencilState::Read, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::DepthStencilState::Reset;
        renderFlags |= RenderFlags::DepthStencilState::Read;
    }
    if (ImGui::Selectable("ReverseZ", renderFlags & RenderFlags::DepthStencilState::ReverseZ, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::DepthStencilState::Reset;
        renderFlags |= RenderFlags::DepthStencilState::ReverseZ;
    }
    if (ImGui::Selectable("ReadReverseZ", renderFlags & RenderFlags::DepthStencilState::ReadReverseZ, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::DepthStencilState::Reset;
        renderFlags |= RenderFlags::DepthStencilState::ReadReverseZ;
    }
    if (ImGui::Selectable("Reset", false, ImGuiSelectableFlags_DontClosePopups))
        renderFlags &= RenderFlags::DepthStencilState::Reset;
}

void DebugUI::RenderFlagsRasterizerState(std::uint32_t& renderFlags) {
    if (ImGui::Selectable("CullNone", renderFlags & RenderFlags::RasterizerState::CullNone, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::RasterizerState::Reset; 
        renderFlags |= RenderFlags::RasterizerState::CullNone;
    }
    if (ImGui::Selectable("CullClockwise", renderFlags & RenderFlags::RasterizerState::CullClockwise, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::RasterizerState::Reset; 
        renderFlags |= RenderFlags::RasterizerState::CullClockwise;
    }
    if (ImGui::Selectable("CullCounterClockwise", renderFlags & RenderFlags::RasterizerState::CullCounterClockwise, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::RasterizerState::Reset; 
        renderFlags |= RenderFlags::RasterizerState::CullCounterClockwise;
    }
    if (ImGui::Selectable("Wireframe", renderFlags & RenderFlags::RasterizerState::Wireframe, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::RasterizerState::Reset; 
        renderFlags |= RenderFlags::RasterizerState::Wireframe;
    }
    if (ImGui::Selectable("Reset", false, ImGuiSelectableFlags_DontClosePopups))
        renderFlags &= RenderFlags::RasterizerState::Reset; 
}

void DebugUI::RenderFlagsSamplerState(std::uint32_t& renderFlags) {
    if (ImGui::Selectable("PointWrap", renderFlags & RenderFlags::SamplerState::PointWrap, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::SamplerState::Reset;
        renderFlags |= RenderFlags::SamplerState::PointWrap;
    }
    if (ImGui::Selectable("PointClamp", renderFlags & RenderFlags::SamplerState::PointClamp, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::SamplerState::Reset;
        renderFlags |= RenderFlags::SamplerState::PointClamp;
    }
    if (ImGui::Selectable("LinearWrap", renderFlags & RenderFlags::SamplerState::LinearWrap, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::SamplerState::Reset;
        renderFlags |= RenderFlags::SamplerState::LinearWrap;
    }
    if (ImGui::Selectable("LinearClamp", renderFlags & RenderFlags::SamplerState::LinearClamp, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::SamplerState::Reset;
        renderFlags |= RenderFlags::SamplerState::LinearClamp;
    }
    if (ImGui::Selectable("AnisotropicWrap", renderFlags & RenderFlags::SamplerState::AnisotropicWrap, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::SamplerState::Reset;
        renderFlags |= RenderFlags::SamplerState::AnisotropicWrap;
    }
    if (ImGui::Selectable("AnisotropicClamp", renderFlags & RenderFlags::SamplerState::AnisotropicClamp, ImGuiSelectableFlags_DontClosePopups)) {
        renderFlags &= RenderFlags::SamplerState::Reset;
        renderFlags |= RenderFlags::SamplerState::AnisotropicClamp;
    }
    if (ImGui::Selectable("Reset", false, ImGuiSelectableFlags_DontClosePopups))
        renderFlags &= RenderFlags::SamplerState::Reset;
}

void DebugUI::RenderFlagsEffects(std::uint32_t& renderFlags) {
    if (ImGui::Selectable("None", renderFlags & RenderFlags::Effect::None, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::None;
    if (ImGui::Selectable("Fog", renderFlags & RenderFlags::Effect::Fog, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::Fog;
    if (ImGui::Selectable("Lighting", renderFlags & RenderFlags::Effect::Lighting, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::Lighting;
    if (ImGui::Selectable("PerPixelLighting", renderFlags & RenderFlags::Effect::PerPixelLighting, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::PerPixelLighting;
    if (ImGui::Selectable("Texture",  renderFlags & RenderFlags::Effect::Texture, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::Texture;
    if (ImGui::Selectable("Instanced", renderFlags & RenderFlags::Effect::Instanced, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::Instanced;
    if (ImGui::Selectable("Specular", renderFlags & RenderFlags::Effect::Specular, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::Specular;
    if (ImGui::Selectable("Skinned", renderFlags & RenderFlags::Effect::Skinned, ImGuiSelectableFlags_DontClosePopups))
        renderFlags ^= RenderFlags::Effect::Skinned;
    if (ImGui::Selectable("Reset", false, ImGuiSelectableFlags_DontClosePopups))
        renderFlags &= RenderFlags::Effect::Reset;
}

void DebugUI::RenderFlags(std::uint32_t& renderFlags) {
    if (ImGui::BeginTabBar("RenderFlags")) {
        if (ImGui::BeginTabItem("Presets")) {
            RenderFlagsPresets(renderFlags);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("BlendState")) {
            RenderFlagsBlendState(renderFlags);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("DepthStencilState")) {
            RenderFlagsDepthStencilState(renderFlags);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("RasterizerState")) {
            RenderFlagsRasterizerState(renderFlags);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("SamplerState")) {
            RenderFlagsSamplerState(renderFlags);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Effects")) {
            RenderFlagsEffects(renderFlags);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void DebugUI::MaterialHeader(Material& material) {
    ImVec2 buttonSize(ImGui::GetFontSize(), ImGui::GetFontSize());
    ImVec4 diffuse  = { material.GetDiffuseColor().x,  material.GetDiffuseColor().y,  material.GetDiffuseColor().z,  material.GetDiffuseColor().w  };
    ImVec4 emissive = { material.GetEmissiveColor().x, material.GetEmissiveColor().y, material.GetEmissiveColor().z, material.GetEmissiveColor().w }; 
    ImVec4 specular = { material.GetSpecularColor().x, material.GetSpecularColor().y, material.GetSpecularColor().z, material.GetSpecularColor().w };

    ImGui::Text("%s", material.GetName().c_str());
    ImGui::SameLine();
    ImGui::ColorButton("diffuse", diffuse, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("emissive", emissive, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("specular", specular, ImGuiColorEditFlags_None, buttonSize);
}

bool DebugUI::SelectableMaterialHeader(bool state, Material& material) {
    bool selected = state;
    ImVec2 buttonSize(ImGui::GetFontSize(), ImGui::GetFontSize());
    ImVec4 diffuse  = { material.GetDiffuseColor().x,  material.GetDiffuseColor().y,  material.GetDiffuseColor().z,  material.GetDiffuseColor().w  };
    ImVec4 emissive = { material.GetEmissiveColor().x, material.GetEmissiveColor().y, material.GetEmissiveColor().z, material.GetEmissiveColor().w }; 
    ImVec4 specular = { material.GetSpecularColor().x, material.GetSpecularColor().y, material.GetSpecularColor().z, material.GetSpecularColor().w };

    ImGui::SetNextItemAllowOverlap();
    if (ImGui::Selectable(material.GetName().c_str(), state))
        selected = true;
    ImGui::SameLine();
    ImGui::ColorButton("diffuse", diffuse, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("emissive", emissive, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("specular", specular, ImGuiColorEditFlags_None, buttonSize);
    return selected;
}

bool DebugUI::TreeNodeMaterialHeader(Material& material) {
    ImVec2 buttonSize(ImGui::GetFontSize(), ImGui::GetFontSize());
    ImVec4 diffuse  = { material.GetDiffuseColor().x,  material.GetDiffuseColor().y,  material.GetDiffuseColor().z,  material.GetDiffuseColor().w  };
    ImVec4 emissive = { material.GetEmissiveColor().x, material.GetEmissiveColor().y, material.GetEmissiveColor().z, material.GetEmissiveColor().w }; 
    ImVec4 specular = { material.GetSpecularColor().x, material.GetSpecularColor().y, material.GetSpecularColor().z, material.GetSpecularColor().w };
    bool open = ImGui::TreeNodeEx(material.GetName().c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
    ImGui::SameLine();
    ImGui::ColorButton("diffuse", diffuse, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("emissive", emissive, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("specular", specular, ImGuiColorEditFlags_None, buttonSize);
    return open;
}

void DebugUI::MaterialSelector(std::uint32_t& index, const std::vector<std::shared_ptr<Material>>& materials) {
    for (std::uint32_t i = 0; i < materials.size(); ++i) {
        if (SelectableMaterialHeader(index == i, *materials[i]))
            index = i;
    }
}

void DebugUI::MaterialSelector(std::uint64_t& GUID, const Materials& materials) {
    for (auto& materialPair : materials) {
        if (SelectableMaterialHeader(materialPair.first == GUID, *materialPair.second))
            GUID = materialPair.first;
    }
}

void DebugUI::MaterialTextures(Material& material) {
    ImGui::Text("Diffuse map: %ws", material.GetDiffuseMapFilePath().c_str());
    ImGui::Text("Normal map:  %ws", material.GetNormalMapFilePath().c_str());
}

void DebugUI::MaterialColors(Material& material) {
    float diffuse[4];
    StoreFloat4(material.GetDiffuseColor(), diffuse);
    float emissive[4];
    StoreFloat4(material.GetEmissiveColor(), emissive);
    float specular[4];
    StoreFloat4(material.GetSpecularColor(), specular);

    if (ImGui::ColorEdit4(GUIDLabel("Diffuse", material.GetGUID()).c_str(), diffuse))
        LoadFloat4(diffuse, material.GetDiffuseColor());
    if (ImGui::ColorEdit4(GUIDLabel("Emissive", material.GetGUID()).c_str(), emissive))
        LoadFloat4(emissive, material.GetEmissiveColor());
    if (ImGui::ColorEdit4(GUIDLabel("Specular", material.GetGUID()).c_str(), specular))
        LoadFloat4(specular, material.GetSpecularColor());
}

void DebugUI::MaterialCreator(AssetBatch& batch) {
    static char diffuseMapFilePath[128] = "";
    static char normalMapFilePath[128] = "";
    static char name[128] = "";
    static std::uint32_t renderFlags = RenderFlags::None;
    static float diffuse[4] = { 1.f, 1.f, 1.f, 1.f };
    static float emissive[4] = { 0.f, 0.f, 0.f, 1.f };
    static float specular[4] = { 1.f, 1.f, 1.f, 1.f };

    ImGui::InputText("Name##MaterialCreator", name, std::size(name));

    ImGui::SeparatorText("Textures");
    bool validDiffuse = InputFilePath("Diffuse map##MaterialCreator", diffuseMapFilePath, std::size(diffuseMapFilePath));
    SameLineError(!validDiffuse, "file not found");
    bool validNormal = InputFilePath("Normal map##MaterialCreator", normalMapFilePath, std::size(normalMapFilePath));
    SameLineError(!validNormal, "file not found");

    ImGui::SeparatorText("Render flags");
    RenderFlags(renderFlags);

    ImGui::SeparatorText("Colors");

    ImGui::ColorEdit4("Diffuse##MaterialCreator", diffuse);
    ImGui::ColorEdit4("Emissive##MaterialCreator", emissive);
    ImGui::ColorEdit4("Specular##MaterialCreator", specular);

    if (ImGui::Button("Create new material##MaterialCreator") && validDiffuse && validNormal) {
        DirectX::XMFLOAT4 D;
        LoadFloat4(diffuse, D);
        DirectX::XMFLOAT4 E;
        LoadFloat4(emissive, E);
        DirectX::XMFLOAT4 S;
        LoadFloat4(specular, S);

        batch.Add(std::make_shared<Material>(
                    AnsiToWString(diffuseMapFilePath),
                    AnsiToWString(normalMapFilePath),
                    std::string(name),
                    renderFlags,
                    DirectX::XMLoadFloat4(&D), 
                    DirectX::XMLoadFloat4(&E),
                    DirectX::XMLoadFloat4(&S)));
    }
}

void DebugUI::MaterialMenu(Material& material) {
    std::uint32_t renderFlags = material.GetFlags();

    ImGui::SeparatorText("Identifiers");
    AssetMenu(material);

    ImGui::SeparatorText("Textures");
    MaterialTextures(material);

    ImGui::SeparatorText("Render flags");
    RenderFlags(renderFlags);

    ImGui::SeparatorText("Colors");
    MaterialColors(material);
}

void DebugUI::MaterialMenu(std::vector<std::shared_ptr<Material>>& materials) {
    for (std::shared_ptr<Material>& pMaterial : materials) {
        if (TreeNodeMaterialHeader(*pMaterial)) {
            MaterialMenu(*pMaterial);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }
}

void DebugUI::MaterialMenu(const std::unordered_map<std::uint64_t, std::shared_ptr<Material>>& materials) {
    for (auto& materialPair : materials) {
        if (TreeNodeMaterialHeader(*materialPair.second)) {
            MaterialMenu(*materialPair.second);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }
}

void DebugUI::MaterialCreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(GUIDLabel("+", "MaterialCreatorPopupMenu").c_str())) 
        ImGui::OpenPopup("MaterialCreatorPopupMenu");
    if (ImGui::BeginPopup("MaterialCreatorPopupMenu")) {
        MaterialCreator(batch);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Sprites.
// ---------------------------------------------------------------- //

void DebugUI::SpritePosition(Sprite& sprite) {
    float origin[2] = { sprite.GetOrigin().x, sprite.GetOrigin().y };
    float offset[2] = { sprite.GetOffset().x, sprite.GetOffset().y };
    float scale[2] = { sprite.GetScale().x, sprite.GetScale().y };
    float layer = sprite.GetLayer();
    float angle = DirectX::XMConvertToDegrees(sprite.GetAngle());

    if (ImGui::DragFloat2(GUIDLabel("Origin", sprite.GetGUID()).c_str(), origin))
        sprite.GetOrigin() = { origin[0], origin[1] };
    if (ImGui::DragFloat2(GUIDLabel("Offset", sprite.GetGUID()).c_str(), offset))
        sprite.GetOffset() = { offset[0], offset[1] };
    if (ImGui::DragFloat2(GUIDLabel("Scale", sprite.GetGUID()).c_str(), scale))
        sprite.GetScale() = { scale[0], scale[1] };
    if (ImGui::DragFloat(GUIDLabel("Layer", sprite.GetGUID()).c_str(), &layer))
        sprite.SetLayer(layer);
    if (ImGui::DragFloat(GUIDLabel("Angle", sprite.GetGUID()).c_str(), &angle))
        sprite.SetAngle(DirectX::XMConvertToRadians(angle));
}

void DebugUI::SpriteCreator(AssetBatch& batch) {
    static char filePath[128] = "";
    static char name[128] = "";
    static float origin[2] = { 0.f, 0.f };
    static float offset[2] = { 0.f, 0.f };
    static float scale[2] = { 1.f, 1.f };
    static float layer = 0.f;
    static float angle = 0.f;
    static float color[4] = { 0.f, 0.f, 0.f, 1.f };
    static bool visible = true;

    ImGui::InputText("Name##SpriteCreator", name, std::size(name));
    ImGui::Checkbox("Visible##SpriteCreator", &visible);
    bool validFilePath = InputFilePath("File path##SpriteCreator", filePath, std::size(filePath));
    SameLineError(!validFilePath, "file not found");

    ImGui::SeparatorText("Position");
    ImGui::DragFloat2("Origin##SpriteCreator", origin);
    ImGui::DragFloat2("Offset##SpriteCreator", offset);
    ImGui::DragFloat2("Scale##SpriteCreator", scale);
    ImGui::DragFloat("Layer##SpriteCreator", &layer);
    ImGui::DragFloat("Angle##SpriteCreator", &angle);

    ImGui::SeparatorText("Color");
    ImGui::DragFloat4("Color##SpriteCreator", color);

    if (ImGui::Button("Create new sprite##SpriteCreator") && validFilePath) {
        DirectX::XMFLOAT4 C = { color[0], color[1], color[2], color[3] };
        DirectX::XMFLOAT2 Or = { origin[0], origin[1] };
        DirectX::XMFLOAT2 Of = { offset[0], offset[1] };
        DirectX::XMFLOAT2 S = { scale[0], scale[1] };
        batch.Add(std::make_shared<Sprite>(
                    AnsiToWString(filePath),
                    std::string(name),
                    Or, Of, S,
                    layer,
                    angle,
                    DirectX::XMLoadFloat4(&C),
                    visible));
    }
}

void DebugUI::SpriteMenu(Sprite& sprite) {
    bool visible = sprite.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        sprite.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    AssetMenu(sprite);

    ImGui::SeparatorText("Textures");
    ImGui::Text("File: %ws", sprite.GetFilePath().c_str());

    ImGui::SeparatorText("Position");
    SpritePosition(sprite);
    float color[4];
    StoreFloat4(sprite.GetColor(), color);

    ImGui::SeparatorText("Color");
    if (ImGui::ColorEdit4(GUIDLabel("Color", sprite.GetGUID()).c_str(), color))
        LoadFloat4(color, sprite.GetColor());
}

void DebugUI::SpriteMenu(const Sprites& sprites) {
    for (auto& spritePair : sprites) {
        if (ImGui::TreeNode(spritePair.second->GetName().c_str())) {
            SpriteMenu(*spritePair.second);
            ImGui::TreePop();
        }
    }
}

void DebugUI::SpriteCreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(GUIDLabel("+", "SpriteCreatorPopupMenu").c_str())) 
        ImGui::OpenPopup("SpriteCreatorPopupMenu");
    if (ImGui::BeginPopup("SpriteCreatorPopupMenu")) {
        SpriteCreator(batch);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Texts.
// ---------------------------------------------------------------- //

void DebugUI::TextCreator(AssetBatch& batch) {
    static char filePath[128] = "";
    static std::string content = "";
    static char name[128] = "";
    static float origin[2] = { 0.f, 0.f };
    static float offset[2] = { 0.f, 0.f };
    static float scale[2] = { 1.f, 1.f };
    static float layer = 0.f;
    static float angle = 0.f;
    static float color[4] = { 0.f, 0.f, 0.f, 1.f };
    static bool visible = true;

    ImGui::InputText("Name##TextCreator", name, std::size(name));
    ImGui::Checkbox("Visible##TextCreator", &visible);
    InputTextMultiline("Contents##TextCreator", &content);
    bool validFilePath = InputFilePath("File path##TextCreator", filePath, std::size(filePath));
    SameLineError(!validFilePath, "file not found");

    ImGui::SeparatorText("Position");
    ImGui::DragFloat2("Origin##TextCreator", origin);
    ImGui::DragFloat2("Offset##TextCreator", offset);
    ImGui::DragFloat2("Scale##TextCreator", scale);
    ImGui::DragFloat("Layer##TextCreator", &layer);
    ImGui::DragFloat("Angle##TextCreator", &angle);

    ImGui::SeparatorText("Color");
    ImGui::DragFloat4("Color##TextCreator", color);

    if (ImGui::Button("Create new sprite##TextCreator") && validFilePath) {
        DirectX::XMFLOAT4 C = { color[0], color[1], color[2], color[3] };
        DirectX::XMFLOAT2 Or = { origin[0], origin[1] };
        DirectX::XMFLOAT2 Of = { offset[0], offset[1] };
        DirectX::XMFLOAT2 S = { scale[0], scale[1] };
        batch.Add(std::make_shared<Text>(
                    AnsiToWString(filePath),
                    AnsiToWString(content),
                    std::string(name),
                    Or, Of, S,
                    layer,
                    angle,
                    DirectX::XMLoadFloat4(&C),
                    visible));
    }
}

void DebugUI::TextMenu(Text& text) {
    static std::string content;
    SpriteMenu(text);
    InputTextMultiline("Content", &content);
    if (ImGui::Button(GUIDLabel("Save content", text.GetGUID()).c_str()))
        text.SetContent(AnsiToWString(content));
}

void DebugUI::TextMenu(const Texts& texts) {
    for (auto& textPair : texts) {
        if (ImGui::TreeNode(textPair.second->GetName().c_str())) {
            TextMenu(*textPair.second);
            ImGui::TreePop();
        }
    }
}

void DebugUI::TextCreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(GUIDLabel("+", "TextCreatorPopupMenu").c_str())) 
        ImGui::OpenPopup("TextCreatorPopupMenu");
    if (ImGui::BeginPopup("TextCreatorPopupMenu")) {
        TextCreator(batch);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Bones.
// ---------------------------------------------------------------- //

void DebugUI::BoneSelector(std::uint32_t& index, std::vector<Bone>& bones) {
    ImGui::BeginChild("bone_hierarchy", ImVec2(ImGui::GetWindowWidth() - 15.f, 200.f), ImGuiChildFlags_None);
    for (std::uint32_t i = 0; i < bones.size(); ++i) {
        std::string text = std::to_string(i) + ": " + bones[i].GetName() + "; " + std::to_string(bones[i].GetParentIndex());
        if (ImGui::Selectable(text.c_str(), index == i))
            index = i;
    }
    ImGui::EndChild();
}

void DebugUI::BoneMenu(Model& model, std::uint32_t boneIndex) {
    std::vector<Bone>& bones = model.GetBones();

    ImGui::SeparatorText("Identifiers");
    AssetMenu(bones[boneIndex]);

    ImGui::SeparatorText("Parent");
    ImGui::Text("Parent: %s", bones[boneIndex].IsRoot() ? "NONE" : bones[bones[boneIndex].GetParentIndex()].GetName().c_str());
    ImGui::Text("Parent index: %d", bones[boneIndex].GetParentIndex());

    if (ImGui::CollapsingHeader("Transform")) {
        AffineTransformation(model.GetBoneMatrices()[boneIndex]);
        ImGui::Separator();
        Matrix(model.GetBoneMatrices()[boneIndex]);
    }

    if (ImGui::CollapsingHeader("Inverse bind pose")) {
        AffineTransformation(model.GetBoneMatrices()[boneIndex]);
        ImGui::Separator();
        Matrix(model.GetInverseBindPoseMatrices()[boneIndex]);
    }
}

// ---------------------------------------------------------------- //
//                          Submeshes.
// ---------------------------------------------------------------- //

void DebugUI::SubmeshInstances(Submesh& submesh) {
    static std::uint32_t index = 0;
    if (index < 0)
        index = 0;
    if (index > submesh.GetNumInstances())
        index = submesh.GetNumInstances() - 1;

    static ImU32 steps = 1;

    std::uint32_t numCulled = submesh.GetNumCulled();

    ImGui::Text("Total instances:   %d", submesh.GetNumInstances());
    ImGui::SameLine();
    HelpMarker("If instancing isn't enabled on the material then the first instance will be used.");
    ImGui::Text("Visible instances: %d", submesh.GetNumVisibleInstances());
    ImGui::Text("Culled instances:  %d", submesh.GetNumCulled());
    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    if (ImGui::InputScalar("Cull", ImGuiDataType_U32, &numCulled, &steps)) {
        if (numCulled <= submesh.GetNumInstances())
            submesh.SetNumberCulled(numCulled);
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    HelpMarker("The last n instances will be culled.");

    ImGui::Separator();
    std::function<void()> onAdd = [&]() {
        submesh.GetInstances().push_back({});
        DirectX::XMStoreFloat3x4(&submesh.GetInstances().back(), DirectX::XMMatrixIdentity());
    };
    std::function<void()> onRemove = [&]() {
        if (index >= 0 && index < submesh.GetNumInstances() && submesh.GetNumInstances() >= 2) {
            std::swap(submesh.GetInstances()[index], submesh.GetInstances().back());
            submesh.GetInstances().pop_back();
        }
    };
    ArrayControls("index##SubmeshInstances", &index, onAdd, onRemove);

    ImGui::Spacing();

    if (index >= 0 && index < submesh.GetNumInstances()) {
        AffineTransformation(submesh.GetInstances()[index]);
        ImGui::Separator();
        Matrix(submesh.GetInstances()[index]);
    }
}

void DebugUI::SubmeshVertexIndexing(Submesh& submesh) {
    static ImU32 steps = 1;

    std::uint32_t indexCount = submesh.GetIndexCount();
    std::uint32_t startIndex = submesh.GetStartIndex();
    std::uint32_t vertexOffset = submesh.GetVertexOffset();

    ImGui::PushItemWidth(ImGui::GetFontSize() * 7);
    if (ImGui::InputScalar("Index count", ImGuiDataType_U32, &indexCount, &steps))
        submesh.SetIndexCount(indexCount);
    if (ImGui::InputScalar("Start index", ImGuiDataType_U32, &startIndex, &steps))
        submesh.SetStartIndex(startIndex);
    if (ImGui::InputScalar("Vertex offset", ImGuiDataType_U32, &vertexOffset, &steps))
        submesh.SetVertexOffset(vertexOffset);
    ImGui::PopItemWidth();
}

void DebugUI::SubmeshCreator(IMesh& iMesh, std::vector<std::shared_ptr<Material>>& availableMaterials) {
    static char name[128] = "";
    static std::uint32_t materialIndex = 0;
    static bool visible = true;

    ImGui::InputText("Name##SubmeshCreator", name, std::size(name));
    ImGui::Checkbox("Visible##SubmeshCreator", &visible);

    ImGui::SeparatorText("Available materials");
    MaterialSelector(materialIndex, availableMaterials);
    if (ImGui::Button("Create new submesh##SubmeshCreator"))
        iMesh.Add(std::make_unique<Submesh>(name, materialIndex, visible));
}

void DebugUI::SubmeshMenu(Submesh& submesh, std::vector<std::shared_ptr<Material>>& availableMaterials) {
    bool visible = submesh.IsVisible();
    if (ImGui::Checkbox(GUIDLabel("Visible", submesh.GetGUID()).c_str(), &visible))
        submesh.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    AssetMenu(submesh);

    if (ImGui::CollapsingHeader("Available materials")) {
        std::uint32_t materialIndex = submesh.GetMaterialIndex();
        MaterialSelector(materialIndex, availableMaterials);
        submesh.SetMaterialIndex(materialIndex);
    }

    if (ImGui::CollapsingHeader("Instancing"))
        SubmeshInstances(submesh);

    if (ImGui::CollapsingHeader("Vertex indexing"))
        SubmeshVertexIndexing(submesh);
}

// ---------------------------------------------------------------- //
//                          Meshes.
// ---------------------------------------------------------------- //

void DebugUI::IMeshCreator(Model& model) {
    static char name[128] = "";
    static bool visible = true;
    static bool skinned = false;

    ImGui::InputText("Name##MeshCreator", name, std::size(name));
    ImGui::Checkbox("Visible##MeshCreator", &visible);
    ImGui::Checkbox("Skinned##MeshCreator", &skinned);

    if (ImGui::Button("Create new mesh##MeshCreator")) {
        std::shared_ptr<IMesh> pIMesh;
        if (skinned)
            pIMesh = std::make_shared<SkinnedMesh>(name, visible);
        else
            pIMesh = std::make_shared<Mesh>(name, visible);
        pIMesh->Add(std::make_unique<Submesh>(std::string(name) + "_submesh"));
        model.Add(std::move(pIMesh));
    }
}

void DebugUI::IMeshMenu(IMesh& iMesh) {
    if (auto pMesh = dynamic_cast<Mesh*>(&iMesh)) {
        bool visible = iMesh.IsVisible();
        if (ImGui::Checkbox(GUIDLabel("Visible", iMesh.GetGUID()).c_str(), &visible))
            iMesh.SetVisible(visible);

        ImGui::SeparatorText("Identifiers");
        AssetMenu(*pMesh);

        if (ImGui::CollapsingHeader("Vertices")) 
            Vertices(pMesh->GetVertices());
    } else if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(&iMesh)) {
        bool visible = iMesh.IsVisible();
        if (ImGui::Checkbox(GUIDLabel("Visible", iMesh.GetGUID()).c_str(), &visible))
            iMesh.SetVisible(visible);

        ImGui::SeparatorText("Identifiers");
        AssetMenu(*pSkinnedMesh);

        if (ImGui::CollapsingHeader("Vertices")) 
            Vertices(pSkinnedMesh->GetVertices());
    }
    if (ImGui::CollapsingHeader("Indices"))
        Indices(iMesh.GetIndices(), iMesh.GetNumVertices());
}

void DebugUI::IMeshCreatorPopupMenu(Model& model) {
    if (ImGui::Button(GUIDLabel("+", "IMeshCreatorPopupMenu").c_str()))
        ImGui::OpenPopup("IMeshCreatorPopupMenu");
    if (ImGui::BeginPopup("IMeshCreatorPopupMenu")) {
        IMeshCreator(model);
        ImGui::EndPopup();
    }
}

void DebugUI::IMeshAddGeoOrSubmeshPopupMenu(IMesh& iMesh, std::vector<std::shared_ptr<Material>>& availableMaterials) {
    if (ImGui::Button(GUIDLabel("+", "IMeshAddGeoOrSubmeshPopupMenu").c_str()))
        ImGui::OpenPopup("IMeshAddGeoOrSubmeshPopupMenu");
    if (ImGui::BeginPopup("IMeshAddGeoOrSubmeshPopupMenu", ImGuiWindowFlags_MenuBar)) {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu(GUIDLabel("Add submesh", "IMeshAddGeoOrSubmeshPopupMenu").c_str())) {
                SubmeshCreator(iMesh, availableMaterials);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(GUIDLabel("Add geometry", "IMeshAddGeoOrSubmeshPopupMenu").c_str())) {
                AddGeoToIMeshCreator(iMesh);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Mesh factory.
// ---------------------------------------------------------------- //

void DebugUI::AddCubeToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddCubeToMesh", &size);
    if (ImGui::Button("Add to mesh##AddCubeToMesh"))
        MeshFactory::AddCube(iMesh, size);
}

void DebugUI::AddBoxToIMeshCreator(IMesh& iMesh) {
    static float size[3] = { 1.f, 1.f, 1.f };
    static bool invertNormal = false;

    ImGui::InputFloat3("Size##AddBoxToMesh", size);
    ImGui::Checkbox("Invert normals##AddBoxToMesh", &invertNormal);

    if (ImGui::Button("Add to mesh##AddBoxToMesh"))
        MeshFactory::AddBox(iMesh, { size[0], size[1], size[2] }, invertNormal);
}

void DebugUI::AddSphereToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static std::uint64_t tessellation = 16;
    static bool invertNormal = false;

    ImGui::InputFloat("Diameter##AddSphereToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddSphereToMesh", ImGuiDataType_U64, &tessellation);
    ImGui::Checkbox("Invert normals##AddSphereToMesh", &invertNormal);

    if (ImGui::Button("Add to mesh##AddSphereToMesh"))
        MeshFactory::AddSphere(iMesh, diameter, tessellation, invertNormal);
}

void DebugUI::AddGeoSphereToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static std::uint64_t tessellation = 3;

    ImGui::InputFloat("Diameter##AddGeoSphereToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddGeoSphereToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddGeoSphereToMesh"))
        MeshFactory::AddGeoSphere(iMesh, diameter, tessellation);
}

void DebugUI::AddCylinderToIMeshCreator(IMesh& iMesh) {
    static float height = 1.f;
    static float diameter = 1.f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Height##AddCylinderToMesh", &height);
    ImGui::InputFloat("Diameter##AddCylinderToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddCylinderToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddCylinderToMesh"))
        MeshFactory::AddCylinder(iMesh, height, diameter, tessellation);
}

void DebugUI::AddConeToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static float height = 1.f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Diameter##AddConeToMesh", &diameter);
    ImGui::InputFloat("Height##AddConeToMesh", &height);
    ImGui::InputScalar("Tessellation##AddConeToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddConeToMesh"))
        MeshFactory::AddCone(iMesh, diameter, height, tessellation);
}

void DebugUI::AddTorusToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static float thickness = .333f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Diameter##AddTorusToMesh", &diameter);
    ImGui::InputFloat("Thickness##AddTorusToMesh", &thickness);
    ImGui::InputScalar("Tessellation##AddTorusToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddTorusToMesh"))
        MeshFactory::AddCone(iMesh, diameter, thickness, tessellation);
}

void DebugUI::AddTetrahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddTetrahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddTetrahedronToMesh"))
        MeshFactory::AddTetrahedron(iMesh, size);
}

void DebugUI::AddOctahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddOctahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddOctahedronToMesh"))
        MeshFactory::AddOctahedron(iMesh, size);
}

void DebugUI::AddDodecahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddDodecahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddDodecahedronToMesh"))
        MeshFactory::AddDodecahedron(iMesh, size);
}

void DebugUI::AddIcosahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddIcosahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddIcosahedronToMesh"))
        MeshFactory::AddIcosahedron(iMesh, size);
}

void DebugUI::AddTeapotToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 
    static std::uint64_t tessellation = 8;

    ImGui::InputFloat("Size##AddTeapotToMesh", &size);
    ImGui::InputScalar("Tessellation##AddTeapotToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddTeapotToMesh"))
        MeshFactory::AddTeapot(iMesh, size, tessellation);
}

void DebugUI::AddGeoToIMeshCreator(IMesh& iMesh) {
    static std::uint8_t selected = 0;
    static const std::string options[12] = { "Cube", "Box", "Sphere", "GeoSphere", "Cylinder", "Cone", "Torus", "Tetrahedron", "Octahedron", "Dodecahedron", "Icosahedron", "Teapot" };

    if (ImGui::Button(GUIDLabel(options[selected], "AddGeoToIMeshCreator").c_str()))
        ImGui::OpenPopup("OutlineTypeSelector");
    if (ImGui::BeginPopup("OutlineTypeSelector")) {
        for (std::uint8_t i = 0; i < std::size(options); ++i) {
            if (ImGui::Selectable(GUIDLabel(options[i], "AddGeoToIMeshCreator").c_str(), selected == i))
                selected = i;
        }
        ImGui::EndPopup();
    }

    switch (selected) {
        case 0:
            AddCubeToIMeshCreator(iMesh); break;
        case 1:
            AddBoxToIMeshCreator(iMesh); break;
        case 2:
            AddSphereToIMeshCreator(iMesh); break;
        case 3:
            AddGeoSphereToIMeshCreator(iMesh); break;
        case 4:
            AddCylinderToIMeshCreator(iMesh); break;
        case 5:
            AddConeToIMeshCreator(iMesh); break;
        case 6:
            AddTorusToIMeshCreator(iMesh); break;
        case 7:
            AddTetrahedronToIMeshCreator(iMesh); break;
        case 8:
            AddOctahedronToIMeshCreator(iMesh); break;
        case 9:
            AddDodecahedronToIMeshCreator(iMesh); break;
        case 10:
            AddIcosahedronToIMeshCreator(iMesh); break;
        default:
            AddTeapotToIMeshCreator(iMesh); break;
    }
}

// ---------------------------------------------------------------- //
//                          Models.
// ---------------------------------------------------------------- //

void DebugUI::ModelSelector(AssetBatch& batch, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh) {
    int expandAll = -1;
    if (ImGui::Button("Expand all"))
        expandAll = 1;
    ImGui::SameLine();
    if (ImGui::Button("Collapse all"))
        expandAll = 0;

    static ImGuiTreeNodeFlags hierarchyBaseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    for (auto modelPair : batch.GetModels()) {
        ImGuiTreeNodeFlags hierarchyNodeFlags = hierarchyBaseNodeFlags;

        if (expandAll != -1)
            ImGui::SetNextItemOpen(expandAll != 0);
        bool modelNodeOpen = ImGui::TreeNodeEx(modelPair.second->GetName().c_str(), hierarchyNodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            *ppSelectedModel = modelPair.second.get();
            *ppSelectedIMesh = nullptr;
            *ppSelectedSubmesh = nullptr;
        }

        if (modelNodeOpen) {
            for (std::shared_ptr<IMesh>& pIMesh : modelPair.second->GetMeshes()) {
                if (expandAll != -1)
                    ImGui::SetNextItemOpen(expandAll != 0);
                bool meshNodeOpen = ImGui::TreeNodeEx(pIMesh->GetName().c_str(), hierarchyNodeFlags);

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                    *ppSelectedModel = modelPair.second.get();
                    *ppSelectedIMesh = pIMesh.get();
                    *ppSelectedSubmesh = nullptr;
                }

                if (meshNodeOpen) {
                    for (std::unique_ptr<Submesh>& pSubmesh : pIMesh->GetSubmeshes()) {
                        hierarchyNodeFlags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                        ImGui::TreeNodeEx(pSubmesh->GetName().c_str(), hierarchyNodeFlags);
                        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                            *ppSelectedModel = modelPair.second.get();
                            *ppSelectedIMesh = pIMesh.get();
                            *ppSelectedSubmesh = pSubmesh.get();
                        }
                    }
                    hierarchyNodeFlags = hierarchyBaseNodeFlags;

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
}

void DebugUI::ModelCreator(AssetBatch& batch) {
    static char name[128] = "";
    static char filePath[128] = "";
    static std::uint64_t baseMaterialGUID = 0;
    static bool visible = true;
    static bool skinned = false;
    static bool packed = true;

    ImGui::InputText("Name##ModelCreator", name, std::size(name));
    ImGui::Checkbox("Visible##ModelCreator", &visible);
    ImGui::Checkbox("Skinned##ModelCreator", &skinned);

    ImGui::SeparatorText("Material");
    MaterialSelector(baseMaterialGUID, batch.GetMaterials());

    ImGui::SeparatorText("Import from file (optional)");
    bool validModel = InputFilePath("Filepath##ModelCreator", filePath, std::size(filePath)); 
    Error(validModel, "Invalid file path");
    ImGui::Checkbox("Pack meshes##ModelCreator", &packed);

    if (ImGui::Button("Create new model##ModelCreator")) {
        std::shared_ptr<Model> pModel;

        if (validModel) {
            pModel = AssetIO::ImportModel(filePath, batch.GetMaterial(baseMaterialGUID), skinned, packed);
        } else {
            pModel = std::make_unique<Model>(
                    batch.GetMaterial(baseMaterialGUID),
                    std::string(name),
                    visible);

            std::shared_ptr<IMesh> pIMesh;
            if (skinned)
                pIMesh = std::make_shared<SkinnedMesh>();
            else
                pIMesh = std::make_shared<Mesh>();
            pIMesh->Add(std::make_unique<Submesh>());

            pModel->Add(std::move(pIMesh));
        }

        batch.Add(std::move(pModel));
    }
}

void DebugUI::ModelMenu(Model& model) {
    bool visible = model.IsVisible();
    if (ImGui::Checkbox(GUIDLabel("Visible", model.GetGUID()).c_str(), &visible))
        model.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    AssetMenu(model);
    if (ImGui::CollapsingHeader("World transform")) {
        HelpMarker("Transformation will be applied to all instances of all submeshes of all meshes in this model.\n!CAUTION! meshes could be shared beteen models.");
        DirectX::XMFLOAT3X4 W;
        if (AffineTransformation(W))
            model.SetWorldTransform(W);
    }

    if (ImGui::CollapsingHeader("Armature")) {
        static std::uint32_t selectedBone = std::uint32_t(-1);
        BoneSelector(selectedBone, model.GetBones());
        if (selectedBone != std::uint32_t(-1) && selectedBone < model.GetNumBones()) {
            ImGui::SeparatorText(model.GetBones()[selectedBone].GetName().c_str());
            BoneMenu(model, selectedBone);
            ImGui::Separator();
        }
    }

    if (ImGui::CollapsingHeader(GUIDLabel("Materials", "ModelMenu").c_str()))
        MaterialMenu(model.GetMaterials());
}

void DebugUI::ModelCreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(GUIDLabel("+", "ModelCreatorPopupMenu").c_str()))
        ImGui::OpenPopup("ModelCreatorPopupMenu");
    if (ImGui::BeginPopup("ModelCreatorPopupMenu")) {
        ModelCreator(batch);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Outlines.
// ---------------------------------------------------------------- //

Outline::Type DebugUI::OutlineTypeSelector() {
    static Outline::Type options[5] = { Outline::Type::Grid, Outline::Type::Ring, Outline::Type::Ray, Outline::Type::Triangle, Outline::Type::Quad };
    static std::string optionsStr[5] = { "Grid", "Ring", "Ray", "Triangle", "Quad" };

    static Outline::Type selected = options[0];
    static std::string selectedStr = optionsStr[0];

    if (ImGui::Button(GUIDLabel(selectedStr, "OutlineTypeSelector").c_str()))
        ImGui::OpenPopup("OutlineTypeSelector");
    if (ImGui::BeginPopup("OutlineTypeSelector")) {
        for (std::uint8_t i = 0; i < std::size(options); ++i) {
            if (ImGui::Selectable(GUIDLabel(optionsStr[i], "OutlineTypeSelector").c_str(), selected == options[i])) {
                selected = options[i];
                selectedStr = optionsStr[i];
            }
        }
        ImGui::EndPopup();
    }
    return selected;
}

void DebugUI::OutlineCreator(AssetBatch& batch) {
    static char name[128] = "";
    static Outline::Type type = Outline::Type::Grid;
    static float color[4] = { 0.f, 0.f, 0.f, 1.f };
    static bool visible = true;

    ImGui::InputText("Name##OutlineCreator", name, std::size(name));
    type = OutlineTypeSelector();
    ImGui::ColorEdit4("Color##OutlineCreator", color);
    ImGui::Checkbox("Visible##OutlineCreator", &visible);

    if (ImGui::Button("Create new outline")) {
        std::shared_ptr<Outline> pOutline;
        DirectX::XMFLOAT4 C = { color[0], color[1], color[2], color[3] };
        switch (type) {
            case Outline::Type::Grid:
                {
                    DirectX::XMFLOAT3 xAxis = { 1.f, 0.f, 0.f };
                    DirectX::XMFLOAT3 yAxis = { 0.f, 0.f, 1.f };
                    DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f };
                    pOutline = std::make_shared<GridOutline>(name, 2, 2, xAxis, yAxis, origin, DirectX::XMLoadFloat4(&C), visible);
                }
                break; 
            case Outline::Type::Ring:
                {
                    DirectX::XMFLOAT3 majorAxis = { .5f, 0.f, 0.f };
                    DirectX::XMFLOAT3 minorAxis = { 0.f, 0.f, .5f };
                    DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f };
                    pOutline = std::make_shared<RingOutline>(name, majorAxis, minorAxis, origin, DirectX::XMLoadFloat4(&C), visible);
                }
                break; 
            case Outline::Type::Ray:
                {
                    DirectX::XMFLOAT3 direction = { 1.f, 0.f, 0.f };
                    DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f };
                    pOutline = std::make_shared<RayOutline>(name, direction, origin, DirectX::XMLoadFloat4(&C), false, visible);
                }
                break; 
            case Outline::Type::Triangle:
                {
                    DirectX::XMFLOAT3 pointA = { 0.f, 0.f, 0.f };
                    DirectX::XMFLOAT3 pointB = { 1.f, 0.f, 0.f };
                    DirectX::XMFLOAT3 pointC = { .5f, 1.f, 0.f };
                    pOutline = std::make_shared<TriangleOutline>(name, pointA, pointB, pointC, DirectX::XMLoadFloat4(&C), visible);
                }
                break; 
            default:
                {
                    DirectX::XMFLOAT3 pointA = { 0.f, 0.f, 0.f };
                    DirectX::XMFLOAT3 pointB = { 0.f, 1.f, 0.f };
                    DirectX::XMFLOAT3 pointC = { 1.f, 0.f, 0.f };
                    DirectX::XMFLOAT3 pointD = { 1.f, 1.f, 0.f };
                    pOutline = std::make_shared<QuadOutline>(name, pointA, pointB, pointC, pointD, DirectX::XMLoadFloat4(&C), visible);
                }
                break; 
        }
        batch.Add(std::move(pOutline));
    }
}

void DebugUI::BoundingBoxOutlineMenu(BoundingBodyOutline<DirectX::BoundingBox>& outline) {
    DirectX::BoundingBox& b = outline.GetBoundingBody();

    float center[3] = { b.Center.x, b.Center.y, b.Center.z };
    float extents[3] = { b.Extents.x, b.Extents.y, b.Extents.z };

    if (ImGui::DragFloat3(GUIDLabel("Center", outline.GetGUID()).c_str(), center))
        b.Center = { center[0], center[1], center[2] };
    if (ImGui::DragFloat3(GUIDLabel("Extents", outline.GetGUID()).c_str(), extents))
        b.Extents = { extents[0], extents[1], extents[2] };
}

void DebugUI::BoundingFrustumOutlineMenu(BoundingBodyOutline<DirectX::BoundingFrustum>& outline) {
    DirectX::BoundingFrustum& b = outline.GetBoundingBody();

    float origin[3] = { b.Origin.x, b.Origin.y, b.Origin.z };

    DirectX::SimpleMath::Quaternion q = b.Orientation;
    DirectX::XMFLOAT3 r = q.ToEuler();
    float rotation[3] = { r.x, r.y, r.z };

    if (ImGui::DragFloat3(GUIDLabel("Origin", outline.GetGUID()).c_str(), origin))
        b.Origin = { origin[0], origin[1], origin[2] };
    if (ImGui::DragFloat3(GUIDLabel("Rotation", outline.GetGUID()).c_str(), rotation))
        DirectX::XMStoreFloat4(&b.Orientation, DirectX::XMQuaternionRotationRollPitchYaw(
                    DirectX::XMConvertToRadians(rotation[0]), 
                    DirectX::XMConvertToRadians(rotation[1]), 
                    DirectX::XMConvertToRadians(rotation[2])));
    ImGui::DragFloat(GUIDLabel("Right slope", outline.GetGUID()).c_str(), &b.RightSlope);
    ImGui::DragFloat(GUIDLabel("Left slope", outline.GetGUID()).c_str(), &b.LeftSlope);
    ImGui::DragFloat(GUIDLabel("Top slope", outline.GetGUID()).c_str(), &b.TopSlope);
    ImGui::DragFloat(GUIDLabel("Bottom slope", outline.GetGUID()).c_str(), &b.BottomSlope);
    ImGui::DragFloat(GUIDLabel("Near", outline.GetGUID()).c_str(), &b.Near);
    ImGui::DragFloat(GUIDLabel("Far", outline.GetGUID()).c_str(), &b.Far);
}

void DebugUI::BoundingOrientedBoxOutlineMenu(BoundingBodyOutline<DirectX::BoundingOrientedBox>& outline) {
    DirectX::BoundingOrientedBox& b = outline.GetBoundingBody();

    float center[3] = { b.Center.x, b.Center.y, b.Center.z };
    float extents[3] = { b.Extents.x, b.Extents.y, b.Extents.z };
    DirectX::SimpleMath::Quaternion q = b.Orientation;
    DirectX::XMFLOAT3 r = q.ToEuler();
    float rotation[3] = { r.x, r.y, r.z };

    if (ImGui::DragFloat3(GUIDLabel("Center", outline.GetGUID()).c_str(), center))
        b.Center = { center[0], center[1], center[2] };
    if (ImGui::DragFloat3(GUIDLabel("Extents", outline.GetGUID()).c_str(), extents))
        b.Extents = { extents[0], extents[1], extents[2] };
    if (ImGui::DragFloat3(GUIDLabel("Rotation", outline.GetGUID()).c_str(), rotation))
        DirectX::XMStoreFloat4(&b.Orientation, DirectX::XMQuaternionRotationRollPitchYaw(
                    DirectX::XMConvertToRadians(rotation[0]), 
                    DirectX::XMConvertToRadians(rotation[1]), 
                    DirectX::XMConvertToRadians(rotation[2])));
}

void DebugUI::BoundingSphereOutlineMenu(BoundingBodyOutline<DirectX::BoundingSphere>& outline) {
    DirectX::BoundingSphere& b = outline.GetBoundingBody();

    float center[3] = { b.Center.x, b.Center.y, b.Center.z };

    if (ImGui::DragFloat3(GUIDLabel("Center", outline.GetGUID()).c_str(), center))
        b.Center = { center[0], center[1], center[2] };
    ImGui::DragFloat(GUIDLabel("Radius", outline.GetGUID()).c_str(), &b.Radius);
}

void DebugUI::GridOutlineMenu(GridOutline& outline) {
    float xDivisions = outline.GetXDivisions();
    float yDivisions = outline.GetYDivisions();
    if (ImGui::DragScalar(GUIDLabel("X divisions", outline.GetGUID()).c_str(), ImGuiDataType_U16, &xDivisions))
        outline.SetXDivisions(xDivisions);
    if (ImGui::DragScalar(GUIDLabel("Y divisions", outline.GetGUID()).c_str(), ImGuiDataType_U16, &yDivisions))
        outline.SetYDivisions(yDivisions);

    float xAxis[3];
    StoreFloat3(outline.GetXAxis(), xAxis);
    float yAxis[3];
    StoreFloat3(outline.GetYAxis(), yAxis);
    float origin[3];
    StoreFloat3(outline.GetOrigin(), origin);

    if (ImGui::DragFloat3(GUIDLabel("X axis", outline.GetGUID()).c_str(), xAxis))
        LoadFloat3(xAxis, outline.GetXAxis());
    if (ImGui::DragFloat3(GUIDLabel("Y axis", outline.GetGUID()).c_str(), yAxis))
        LoadFloat3(yAxis, outline.GetYAxis());
    if (ImGui::DragFloat3(GUIDLabel("Origin", outline.GetGUID()).c_str(), origin))
        LoadFloat3(origin, outline.GetOrigin());
}

void DebugUI::RingOutlineMenu(RingOutline& outline) {
    float minorAxis[3];
    StoreFloat3(outline.GetMinorAxis(), minorAxis);
    float majorAxis[3];
    StoreFloat3(outline.GetMajorAxis(), majorAxis);
    float origin[3];
    StoreFloat3(outline.GetOrigin(), origin);

    if (ImGui::DragFloat3(GUIDLabel("Minor axis", outline.GetGUID()).c_str(), minorAxis))
        LoadFloat3(minorAxis, outline.GetMinorAxis());
    if (ImGui::DragFloat3(GUIDLabel("Major axis", outline.GetGUID()).c_str(), majorAxis))
        LoadFloat3(majorAxis, outline.GetMajorAxis());
    if (ImGui::DragFloat3(GUIDLabel("Origin", outline.GetGUID()).c_str(), origin))
        LoadFloat3(origin, outline.GetOrigin());
}

void DebugUI::RayOutlineMenu(RayOutline& outline) {
    bool normalized = outline.IsNormalized();
    if (ImGui::Checkbox(GUIDLabel("Normalized", outline.GetGUID()).c_str(), &normalized))
        outline.SetNormalized(normalized);

    float direction[3];
    StoreFloat3(outline.GetDirection(), direction);
    float origin[3];
    StoreFloat3(outline.GetOrigin(), origin);

    if (ImGui::DragFloat3(GUIDLabel("Direction", outline.GetGUID()).c_str(), direction))
        LoadFloat3(direction, outline.GetDirection());
    if (ImGui::DragFloat3(GUIDLabel("Origin##", outline.GetGUID()).c_str(), origin))
        LoadFloat3(origin, outline.GetOrigin());
}

void DebugUI::TriangleOutlineMenu(TriangleOutline& outline) {
    float pointA[3];
    StoreFloat3(outline.GetPointA(), pointA);
    float pointB[3];
    StoreFloat3(outline.GetPointB(), pointB);
    float pointC[3];
    StoreFloat3(outline.GetPointC(), pointC);

    if (ImGui::DragFloat3(GUIDLabel("Point A", outline.GetGUID()).c_str(), pointA))
        LoadFloat3(pointA, outline.GetPointA());
    if (ImGui::DragFloat3(GUIDLabel("Point B", outline.GetGUID()).c_str(), pointB))
        LoadFloat3(pointB, outline.GetPointB());
    if (ImGui::DragFloat3(GUIDLabel("Point C", outline.GetGUID()).c_str(), pointC))
        LoadFloat3(pointC, outline.GetPointC());
}

void DebugUI::QuadOutlineMenu(QuadOutline& outline) {
    float pointA[3];
    StoreFloat3(outline.GetPointA(), pointA);
    float pointB[3];
    StoreFloat3(outline.GetPointB(), pointB);
    float pointC[3];
    StoreFloat3(outline.GetPointC(), pointC);
    float pointD[3];
    StoreFloat3(outline.GetPointD(), pointD);

    if (ImGui::DragFloat3(GUIDLabel("Point A", outline.GetGUID()).c_str(), pointA))
        LoadFloat3(pointA, outline.GetPointA());
    if (ImGui::DragFloat3(GUIDLabel("Point B", outline.GetGUID()).c_str(), pointB))
        LoadFloat3(pointB, outline.GetPointB());
    if (ImGui::DragFloat3(GUIDLabel("Point C", outline.GetGUID()).c_str(), pointC))
        LoadFloat3(pointC, outline.GetPointC());
    if (ImGui::DragFloat3(GUIDLabel("Point D", outline.GetGUID()).c_str(), pointD))
        LoadFloat3(pointD, outline.GetPointD());
}

void DebugUI::OutlineMenu(Outline& outline) {
    bool visible = outline.IsVisible();
    if (ImGui::Checkbox(GUIDLabel("Visible", outline.GetGUID()).c_str(), &visible))
        outline.SetVisible(visible);

    ImGui::SeparatorText("Identifiers");
    AssetMenu(outline);

    if (ImGui::CollapsingHeader("Position")) {
        switch (outline.GetType()) {
            case Outline::Type::BoundingBox: 
                BoundingBoxOutlineMenu(static_cast<BoundingBodyOutline<DirectX::BoundingBox>&>(outline));
                break;
            case Outline::Type::BoundingFrustum:
                BoundingFrustumOutlineMenu(static_cast<BoundingBodyOutline<DirectX::BoundingFrustum>&>(outline));
                break;
            case Outline::Type::BoundingOrientedBox:
                BoundingOrientedBoxOutlineMenu(static_cast<BoundingBodyOutline<DirectX::BoundingOrientedBox>&>(outline));
                break;
            case Outline::Type::BoundingSphere:
                BoundingSphereOutlineMenu(static_cast<BoundingBodyOutline<DirectX::BoundingSphere>&>(outline));
                break;
            case Outline::Type::Grid: 
                GridOutlineMenu(static_cast<GridOutline&>(outline));
                break;
            case Outline::Type::Ring:
                RingOutlineMenu(static_cast<RingOutline&>(outline));
                break;
            case Outline::Type::Ray:
                RayOutlineMenu(static_cast<RayOutline&>(outline));
                break;
            case Outline::Type::Triangle:
                TriangleOutlineMenu(static_cast<TriangleOutline&>(outline));
                break;
            case Outline::Type::Quad:
                QuadOutlineMenu(static_cast<QuadOutline&>(outline));
                break;
        }
    }

    float color[4];
    StoreFloat4(outline.GetColor(), color);
    if (ImGui::CollapsingHeader("Color"))
        if (ImGui::ColorEdit4(GUIDLabel("Color", outline.GetGUID()).c_str(), color))
            LoadFloat4(color, outline.GetColor());
}

void DebugUI::OutlineMenu(const Outlines& outlines) {
    for (auto& outlinePair : outlines) {
        if (ImGui::TreeNode(outlinePair.second->GetName().c_str())) {
            OutlineMenu(*outlinePair.second);
            ImGui::TreePop();
        }
    }
}

void DebugUI::OutlineCreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(GUIDLabel("+", "AddOutlineToBatch").c_str())) 
        ImGui::OpenPopup("AddOutlineToBatch");
    if (ImGui::BeginPopup("AddOutlineToBatch")) {
        OutlineCreator(batch);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------- //
//                          Camera.
// ---------------------------------------------------------------- //

void DebugUI::CameraMenu(Camera& camera) {
    ImGuiViewport* pViewport = ImGui::GetMainViewport();
    static float z[2] = { camera.GetNearZ(), camera.GetFarZ() };
    static float windowSize[2] = { pViewport->WorkSize.x, pViewport->WorkSize.y };
    static float fovY = camera.GetFovY();

    if (ImGui::Button("Reset")) {
        z[0] = .1f; z[1] = 2000.f;
        windowSize[0] = pViewport->WorkSize.x; windowSize[1] = pViewport->WorkSize.y;
        fovY = DirectX::XM_PIDIV4;
    }

    ImGui::SameLine();

    bool isOrthographic = camera.IsOrthographic();
    ImGui::Checkbox("Orthographic", &isOrthographic);
    
    ImGui::SeparatorText("Projection");
    ImGui::DragFloat2("Z planes", z, 1.f, .01f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Text("Fov x: %f", camera.GetFovX());
    ImGui::DragFloat("Fov y", &fovY, DirectX::XMConvertToRadians(.5f), .01f, std::numeric_limits<float>::max());

    ImGui::SeparatorText("Window size");
    ImGui::Text("Aspect: %f", camera.GetAspect());
    ImGui::DragFloat2("Window size", windowSize, 1.f, .1f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Text("Near plane width:  %f", camera.GetNearWindowWidth());
    ImGui::Text("Near plane height: %f", camera.GetNearWindowHeight());
    ImGui::Spacing();
    ImGui::Text("Far plane width:   %f", camera.GetFarWindowWidth());
    ImGui::Text("Far plane height:  %f", camera.GetFarWindowHeight());

    if (isOrthographic)
        camera.SetOrthographicView(windowSize[0], windowSize[1], z[0], std::max(z[1], 1.f));
    else
        camera.SetPerspectiveView(fovY, windowSize[0] / windowSize[1], z[0], std::max(z[1], 1.f));
    camera.Update();
}

// ---------------------------------------------------------------- //
//                          Asset batch.
// ---------------------------------------------------------------- //

void DebugUI::AssetBatchSelector(std::uint8_t& index, std::vector<std::shared_ptr<AssetBatch>>& batches) {
    if (ImGui::BeginListBox("##Batches")) {
        for (std::uint8_t i = 0; i < batches.size(); ++i) {
            if (ImGui::Selectable(batches[i]->GetName().c_str(), index == i))
                index = i;
            if (index == i)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }
}

void DebugUI::AssetBatchStats(AssetBatch& batch) {
    ImGui::Text("Models:                     %llu", batch.GetNumModels());
    ImGui::Text("Meshes:                     %llu", batch.GetNumMeshes());
    ImGui::Text("Submeshes:                  %llu", batch.GetNumSubmeshes());

    ImGui::Separator();
    ImGui::Text("Max unique textures:        %d",   batch.GetMaxNumUniqueTextures());
    ImGui::Text("Unique textures:            %d",   batch.GetNumUniqueTextures());
    ImGui::Text("Materials:                  %llu", batch.GetNumMaterials());
    ImGui::Text("Sprites:                    %llu", batch.GetNumSprites());
    ImGui::Text("Texts:                      %llu", batch.GetNumTexts());
    ImGui::Text("Outlines:                   %llu", batch.GetNumOutlines());

    ImGui::Separator();
    ImGui::Text("Submesh instances:          %llu", batch.GetNumSubmeshInstances());
    ImGui::Text("Rendered submesh instances: %llu", batch.GetNumRenderedSubmeshInstances());
    ImGui::Text("Loaded vertices:            %llu", batch.GetNumLoadedVertices());
    ImGui::Text("Rendered vertices:          %llu", batch.GetNumRenderedVertices());
}

void DebugUI::AssetBatchMenu(AssetBatch& batch) {
    int ID = 0;

    static Model* pSelectedModel = nullptr;
    static IMesh* pSelectedIMesh = nullptr;
    static Submesh* pSelectedSubmesh = nullptr;

    bool visible = batch.IsVisible();
    if (ImGui::Checkbox(GUIDLabel("Visible", batch.GetName()).c_str(), &visible))
        batch.SetVisible(visible);

    if (ImGui::CollapsingHeader("Models")) {
        ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)ID++), ImVec2(ImGui::GetWindowWidth() - 15.f, 200.f), ImGuiChildFlags_None);
        ModelCreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetRemoverPopupMenu(AssetBatch::AssetType::Model, batch);
        ImGui::SameLine();
        ModelSelector(batch, &pSelectedModel, &pSelectedIMesh, &pSelectedSubmesh);
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("%s > %s > %s", 
                pSelectedModel ? pSelectedModel->GetName().c_str() : "...", 
                pSelectedIMesh ? pSelectedIMesh->GetName().c_str() : "...",
                pSelectedSubmesh ? pSelectedSubmesh->GetName().c_str() : "...");
        ImGui::SameLine();
        HelpMarker("Model > Mesh > Submesh");

        if (pSelectedSubmesh)
            SubmeshMenu(*pSelectedSubmesh, pSelectedModel->GetMaterials());
        else if (pSelectedIMesh) {
            IMeshAddGeoOrSubmeshPopupMenu(*pSelectedIMesh, pSelectedModel->GetMaterials());
            ImGui::SameLine();
            IMeshMenu(*pSelectedIMesh);
        }
        else if (pSelectedModel) {
            IMeshCreatorPopupMenu(*pSelectedModel);
            ImGui::SameLine();
            ModelMenu(*pSelectedModel);
        }

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Materials")) {
        MaterialCreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetRemoverPopupMenu(AssetBatch::AssetType::Material, batch);

        MaterialMenu(batch.GetMaterials());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Sprites")) {
        SpriteCreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetRemoverPopupMenu(AssetBatch::AssetType::Sprite, batch);

        SpriteMenu(batch.GetSprites());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Text")) {
        TextCreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetRemoverPopupMenu(AssetBatch::AssetType::Text, batch);

        TextMenu(batch.GetTexts());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader("Outlines")) {
        OutlineCreatorPopupMenu(batch);
        ImGui::SameLine();
        AssetRemoverPopupMenu(AssetBatch::AssetType::Text, batch);

        OutlineMenu(batch.GetOutlines());

        ImGui::Separator();
        ImGui::Spacing();
    }

    if (ImGui::CollapsingHeader(std::string("Stats##" + batch.GetName()).c_str())) {
        AssetBatchStats(batch);

        ImGui::Separator();
        ImGui::Spacing();
    }
}

// ---------------------------------------------------------------- //
//                          Scene.
// ---------------------------------------------------------------- //

void DebugUI::SceneStats(Scene& scene) {
    ImGui::Text("Batches:                    %d"  , scene.GetNumAssetBatches());

    ImGui::Separator();
    ImGui::Text("Models:                     %llu", scene.GetNumModels());
    ImGui::Text("Meshes:                     %llu", scene.GetNumMeshes());
    ImGui::Text("Submeshes:                  %llu", scene.GetNumSubmeshes());

    ImGui::Separator();
    ImGui::Text("Materials:                  %llu", scene.GetNumMaterials());
    ImGui::Text("Sprites:                    %llu", scene.GetNumSprites());
    ImGui::Text("Texts:                      %llu", scene.GetNumTexts());
    ImGui::Text("Outlines:                   %llu", scene.GetNumOutlines());

    ImGui::Separator();
    ImGui::Text("Submesh instances:          %llu", scene.GetNumSubmeshInstances());
    ImGui::Text("Rendered submesh instances: %llu", scene.GetNumRenderedSubmeshInstances());
    ImGui::Text("Loaded vertices:            %llu", scene.GetNumLoadedVertices());
    ImGui::Text("Rendered vertices:          %llu", scene.GetNumRenderedVertices());
}

void DebugUI::SceneMenu(Scene& scene) {
    static std::uint8_t selectedBatch = 0;

    ImGui::SeparatorText("Batches");
    AssetBatchSelector(selectedBatch, scene.GetAssetBatches());
    AssetBatchMenu(*scene.GetAssetBatches()[selectedBatch]);

    ImGui::SeparatorText("Scene");
    if (ImGui::CollapsingHeader("Camera")) {
        CameraMenu(scene.GetCamera());
        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Stats")) {
        SceneStats(scene);
        ImGui::Separator();
        ImGui::Spacing();
    }
}

void DebugUI::SceneWindow(Scene& scene, ImGuiWindowFlags windowFlags) {
    IMGUI_CHECKVERSION();

    static bool open = true;
    if (!ImGui::Begin(scene.GetName().c_str(), &open, windowFlags)) {
        ImGui::End();
        return;
    }
    SceneMenu(scene);
    ImGui::End();
}

