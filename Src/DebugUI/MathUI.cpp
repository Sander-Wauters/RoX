#include "DebugUI/MathUI.h"

#include <functional>

#include <SimpleMath.h>
#include <ImGui/imgui.h>

#include "DebugUI/Util.h"
#include "DebugUI/GeneralUI.h"

void MathUI::Vertex(VertexPositionNormalTexture& vertex) {
    float position[3];
    Util::StoreFloat3(vertex.position, position);
    float normal[3];
    Util::StoreFloat3(vertex.normal, normal);
    float texture[2];
    Util::StoreFloat2(vertex.textureCoordinate, texture);

    if (ImGui::DragFloat3("position", position))
        Util::LoadFloat3(position, vertex.position);
    if (ImGui::DragFloat3("normal", normal))
        Util::LoadFloat3(normal, vertex.normal);
    if (ImGui::DragFloat2("texture", texture))
        Util::LoadFloat2(texture, vertex.textureCoordinate);
}

void MathUI::Vertex(VertexPositionNormalTextureSkinning& vertex) {
    float position[3];
    Util::StoreFloat3(vertex.position, position);
    float normal[3];
    Util::StoreFloat3(vertex.normal, normal);
    float texture[2];
    Util::StoreFloat2(vertex.textureCoordinate, texture);
    float weights[4];
    Util::StoreFloat4(vertex.weights, weights);

    std::uint32_t indices[4] = { vertex.indices & 0xff, (vertex.indices >> 8) & 0xff, (vertex.indices >> 16) & 0xff, (vertex.indices >> 24) & 0xff };

    if (ImGui::DragFloat3("position", position))
        Util::LoadFloat3(position, vertex.position);
    if (ImGui::DragFloat3("normal", normal))
        Util::LoadFloat3(normal, vertex.normal);
    if (ImGui::DragFloat2("texture", texture))
        Util::LoadFloat2(texture, vertex.textureCoordinate);
    if (ImGui::DragScalarN("blend indices", ImGuiDataType_U32, indices, 4))
        vertex.SetBlendIndices({ indices[0], indices[1], indices[2], indices[3] });
    if (ImGui::DragFloat4("blend weights", weights, .25f, 0.f, 0.f, "%.5f"))
        vertex.SetBlendWeights({ weights[0], weights[1], weights[2], weights[3] });
}

void MathUI::Vertices(std::vector<VertexPositionNormalTexture>& vertices) {
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
    GeneralUI::ArrayControls("index##Vertices", &index, onAdd, onRemove);
    if (index >= 0 && index < vertices.size())
        Vertex(vertices[index]);
}

void MathUI::Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices) {
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

    GeneralUI::ArrayControls("index##Vertices", &index, onAdd, onRemove);
    if (index >= 0 && index < vertices.size())
        Vertex(vertices[index]);
}

void MathUI::Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices) {
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
    GeneralUI::ArrayControls("index##Indices", &index, onAdd, onRemove);
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

void MathUI::Matrix(DirectX::XMMATRIX& matrix) {
    DirectX::XMFLOAT4X4 M;
    DirectX::XMStoreFloat4x4(&M, matrix);
    Matrix(M);
    matrix = DirectX::XMLoadFloat4x4(&M);
}

void MathUI::Matrix(DirectX::XMFLOAT4X4& matrix) {
    ImGui::DragFloat4("r0", matrix.m[0], 0.25f);
    ImGui::SameLine();
    GeneralUI::HelpMarker("matrix row-major");
    ImGui::DragFloat4("r1", matrix.m[1], 0.25f);
    ImGui::DragFloat4("r2", matrix.m[2], 0.25f);
    ImGui::DragFloat4("r3", matrix.m[3], 0.25f);
}

void MathUI::Matrix(DirectX::XMFLOAT3X4& matrix) {
    static float r3[4] = { 0.f, 0.f, 0.f, 1.f };
    ImGui::DragFloat4("c0", matrix.m[0], 0.25f);
    ImGui::SameLine();
    GeneralUI::HelpMarker("matrix column-major");
    ImGui::DragFloat4("c1", matrix.m[1], 0.25f);
    ImGui::DragFloat4("c2", matrix.m[2], 0.25f);
    ImGui::InputFloat4("c3", r3, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

bool MathUI::AffineTransformation(DirectX::XMMATRIX& matrix) {
    bool transformationApplied = false;

    static bool applyInRealTime = false;

    static float origin[3] = { 0.f, 0.f, 0.f};
    static float translation[3] = { 0.f, 0.f, 0.f };
    static float scale[3] = { 1.f, 1.f, 1.f };
    static float rotation[3] = { 0.f, 0.f, 0.f };

    if (ImGui::Button("Apply") || applyInRealTime) {
        DirectX::XMVECTOR o = DirectX::XMVectorSet(origin[0], origin[1], origin[2], 0.f);
        DirectX::XMVECTOR t = DirectX::XMVectorSet(translation[0], translation[1], translation[2], 0.f);
        DirectX::XMVECTOR s = DirectX::XMVectorSet(scale[0], scale[1], scale[2], 0.f);
        DirectX::XMVECTOR r = DirectX::XMQuaternionRotationRollPitchYaw(
                DirectX::XMConvertToRadians(rotation[0]), 
                DirectX::XMConvertToRadians(rotation[1]), 
                DirectX::XMConvertToRadians(rotation[2]));
        matrix = DirectX::XMMatrixAffineTransformation(s, o, r, t);
        transformationApplied = true;
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
    ImGui::SameLine();
    ImGui::Checkbox("Apply in real time", &applyInRealTime);
    GeneralUI::HelpMarker("May result in poor performance if left checked on multiple objects.");

    ImGui::DragFloat3("origin", origin, 0.01f);
    ImGui::DragFloat3("translation", translation, 0.1f);
    ImGui::DragFloat3("scale", scale, 0.01f);
    ImGui::DragFloat3("rotation", rotation);

    return transformationApplied;
}

bool MathUI::AffineTransformation(DirectX::XMFLOAT4X4& matrix) {
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&matrix);
    bool transformatonApplied = AffineTransformation(M);
    DirectX::XMStoreFloat4x4(&matrix, M);
    return transformatonApplied;
}

bool MathUI::AffineTransformation(DirectX::XMFLOAT3X4& matrix) {
    DirectX::XMMATRIX M = DirectX::XMLoadFloat3x4(&matrix);
    bool transformatonApplied = AffineTransformation(M);
    DirectX::XMStoreFloat3x4(&matrix, M);
    return transformatonApplied;
}

void MathUI::Vector(std::string label, DirectX::XMFLOAT2& vector, float dragSpeed) {
    float v[2];
    Util::StoreFloat2(vector, v);

    if (ImGui::DragFloat2(label.c_str(), v, dragSpeed))
        Util::LoadFloat2(v, vector);
}

void MathUI::Vector(std::string label, DirectX::XMFLOAT3& vector, float dragSpeed) {
    float v[3];
    Util::StoreFloat3(vector, v);

    if (ImGui::DragFloat3(label.c_str(), v, dragSpeed))
        Util::LoadFloat3(v, vector);
}

void MathUI::Vector(std::string label, DirectX::XMFLOAT4& vector, float dragSpeed) {
    float v[4];
    Util::StoreFloat4(vector, v);

    if (ImGui::DragFloat4(label.c_str(), v, dragSpeed))
        Util::LoadFloat4(v, vector);
}

void MathUI::ColorVector(std::string label, DirectX::XMFLOAT3& colorVector, float dragSpeed) {
    float color[3];
    Util::StoreFloat3(colorVector, color);

    if (ImGui::ColorEdit3(label.c_str(), color, dragSpeed))
        Util::LoadFloat3(color, colorVector);
}

void MathUI::ColorVector(std::string label, DirectX::XMFLOAT4& colorVector, float dragSpeed) {
    float color[4];
    Util::StoreFloat4(colorVector, color);

    if (ImGui::ColorEdit4(label.c_str(), color, dragSpeed))
        Util::LoadFloat4(color, colorVector);
}

void MathUI::QuaternionWithEulerControlls(std::string label, DirectX::XMFLOAT4& quaternion, float dragSpeed) {
    DirectX::SimpleMath::Quaternion q = quaternion;
    DirectX::XMFLOAT3 euler = q.ToEuler();
    float rotation[3] = {
        DirectX::XMConvertToDegrees(euler.x),
        DirectX::XMConvertToDegrees(euler.y),
        DirectX::XMConvertToDegrees(euler.z),
    };

    if (ImGui::DragFloat3(label.c_str(), rotation, dragSpeed))
        DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYaw(
                    DirectX::XMConvertToRadians(rotation[0]), 
                    DirectX::XMConvertToRadians(rotation[1]), 
                    DirectX::XMConvertToRadians(rotation[2])));
}

