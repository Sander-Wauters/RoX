#include "RoX/DebugUI.h"

#include "Util/pch.h"
#include "Util/dxtk12pch.h"

#undef max
#include "ImGui/imgui.h"

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

bool InputTextMultilineImpl(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr) {
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool DebugUI::InputTextMultiline(const char* label, std::string* str) {
    return InputTextMultilineImpl(label, str);
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

void DebugUI::Vertex(VertexPositionNormalTexture& vertex) {
    DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&vertex.position);
    DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&vertex.normal);
    DirectX::XMVECTOR texture = DirectX::XMLoadFloat2(&vertex.textureCoordinate);

    if (ImGui::DragFloat3("position", position.m128_f32))
        DirectX::XMStoreFloat3(&vertex.position, position);
    if (ImGui::DragFloat3("normal", normal.m128_f32))
        DirectX::XMStoreFloat3(&vertex.normal, normal);
    if (ImGui::DragFloat2("texture", texture.m128_f32))
        DirectX::XMStoreFloat2(&vertex.textureCoordinate, texture);
}

void DebugUI::Vertex(VertexPositionNormalTextureSkinning& vertex) {
    DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&vertex.position);
    DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&vertex.normal);
    DirectX::XMVECTOR texture = DirectX::XMLoadFloat2(&vertex.textureCoordinate);
    DirectX::XMVECTOR weights = DirectX::XMLoadFloat4(&vertex.weights);

    std::uint32_t indices[4] = { vertex.indices & 0xff, (vertex.indices >> 8) & 0xff, (vertex.indices >> 16) & 0xff, (vertex.indices >> 24) & 0xff };

    if (ImGui::DragFloat3("position", position.m128_f32))
        DirectX::XMStoreFloat3(&vertex.position, position);
    if (ImGui::DragFloat3("normal", normal.m128_f32))
        DirectX::XMStoreFloat3(&vertex.normal, normal);
    if (ImGui::DragFloat2("texture", texture.m128_f32))
        DirectX::XMStoreFloat2(&vertex.textureCoordinate, texture);
    if (ImGui::DragScalarN("blend indices", ImGuiDataType_U32, indices, 4))
        vertex.SetBlendIndices({ indices[0], indices[1], indices[2], indices[3] });
    if (ImGui::DragFloat4("blend weights", weights.m128_f32, .25f, 0.f, 0.f, "%.5f"))
        vertex.SetBlendWeights({{ weights.m128_f32[0], weights.m128_f32[1], weights.m128_f32[2], weights.m128_f32[3] }});
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

void DebugUI::Matrix(DirectX::XMMATRIX& matrix) {
    ImGui::DragFloat4("r0", matrix.r[0].m128_f32, 0.25f);
    ImGui::SameLine();
    HelpMarker("matrix row-major");
    ImGui::DragFloat4("r1", matrix.r[1].m128_f32, 0.25f);
    ImGui::DragFloat4("r2", matrix.r[2].m128_f32, 0.25f);
    ImGui::DragFloat4("r3", matrix.r[3].m128_f32, 0.25f);
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

        origin[0]      = 0.f;           origin[1]      = 0.f;           origin[2]      = 0.f;
        translation[0] = t.m128_f32[0]; translation[1] = t.m128_f32[1]; translation[2] = t.m128_f32[2];
        scale[0]       = s.m128_f32[0]; scale[1]       = s.m128_f32[1]; scale[2]       = s.m128_f32[2];
        rotation[0]    = r.m128_f32[0]; rotation[1]    = r.m128_f32[1]; rotation[2]    = r.m128_f32[2];
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

void DebugUI::RenderFlags(std::uint32_t renderFlags) {
    if (ImGui::BeginTabBar("RenderFlags", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("BlendState")) {
            ImGui::Selectable("Opaque",           renderFlags & RenderFlags::BlendState::Opaque);
            ImGui::Selectable("AlphaBlend",       renderFlags & RenderFlags::BlendState::AlphaBlend);
            ImGui::Selectable("Additive",         renderFlags & RenderFlags::BlendState::Additive);
            ImGui::Selectable("NonPremultiplied", renderFlags & RenderFlags::BlendState::NonPremultiplied);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("DepthStencilState")) {
            ImGui::Selectable("None",         renderFlags & RenderFlags::DepthStencilState::None);
            ImGui::Selectable("Default",      renderFlags & RenderFlags::DepthStencilState::Default);
            ImGui::Selectable("Read",         renderFlags & RenderFlags::DepthStencilState::Read);
            ImGui::Selectable("ReverseZ",     renderFlags & RenderFlags::DepthStencilState::ReverseZ);
            ImGui::Selectable("ReadReverseZ", renderFlags & RenderFlags::DepthStencilState::ReadReverseZ);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("RasterizerState")) {
            ImGui::Selectable("CullNone",             renderFlags & RenderFlags::RasterizerState::CullNone);
            ImGui::Selectable("CullClockwise",        renderFlags & RenderFlags::RasterizerState::CullClockwise);
            ImGui::Selectable("CullCounterClockwise", renderFlags & RenderFlags::RasterizerState::CullCounterClockwise);
            ImGui::Selectable("Wireframe",            renderFlags & RenderFlags::RasterizerState::Wireframe);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("SamplerState")) {
            ImGui::Selectable("PointWrap",        renderFlags & RenderFlags::SamplerState::PointWrap);
            ImGui::Selectable("PointClamp",       renderFlags & RenderFlags::SamplerState::PointClamp);
            ImGui::Selectable("LinearWrap",       renderFlags & RenderFlags::SamplerState::LinearWrap);
            ImGui::Selectable("LinearClamp",      renderFlags & RenderFlags::SamplerState::LinearClamp);
            ImGui::Selectable("AnisotropicWrap",  renderFlags & RenderFlags::SamplerState::AnisotropicWrap);
            ImGui::Selectable("AnisotropicClamp", renderFlags & RenderFlags::SamplerState::AnisotropicClamp);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Effect")) {
            ImGui::Selectable("None",             renderFlags & RenderFlags::Effect::None);
            ImGui::Selectable("Fog",              renderFlags & RenderFlags::Effect::Fog);
            ImGui::Selectable("Lighting",         renderFlags & RenderFlags::Effect::Lighting);
            ImGui::Selectable("PerPixelLighting", renderFlags & RenderFlags::Effect::PerPixelLighting);
            ImGui::Selectable("Texture",          renderFlags & RenderFlags::Effect::Texture);
            ImGui::Selectable("Instanced",        renderFlags & RenderFlags::Effect::Instanced);
            ImGui::Selectable("Specular",         renderFlags & RenderFlags::Effect::Specular);
            ImGui::Selectable("Skinned",          renderFlags & RenderFlags::Effect::Skinned);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void DebugUI::MaterialTextures(Material& material) {
    ImGui::Text("Diffuse map: %ws", material.GetDiffuseMapFilePath().c_str());
    ImGui::Text("Normal map:  %ws", material.GetNormalMapFilePath().c_str());
}

void DebugUI::MaterialColors(Material& material) {
    ImGui::ColorEdit4(std::string("diffuse##" + material.GetName()).c_str(), material.GetDiffuseColor().m128_f32);
    ImGui::ColorEdit4(std::string("emissive##" + material.GetName()).c_str(), material.GetEmissiveColor().m128_f32);
    ImGui::ColorEdit4(std::string("specular##" + material.GetName()).c_str(), material.GetSpecularColor().m128_f32);
}

void DebugUI::MaterialSelector(std::uint32_t& index, std::vector<std::shared_ptr<Material>>& materials) {
    ImVec2 buttonSize(ImGui::GetFontSize(), ImGui::GetFontSize());

    for (std::uint32_t i = 0; i < materials.size(); ++i) {
        ImVec4 diffuse  = { materials[i]->GetDiffuseColor().m128_f32[0], materials[i]->GetDiffuseColor().m128_f32[1], materials[i]->GetDiffuseColor().m128_f32[2], materials[i]->GetDiffuseColor().m128_f32[3] };
        ImVec4 emissive = { materials[i]->GetEmissiveColor().m128_f32[0], materials[i]->GetEmissiveColor().m128_f32[1], materials[i]->GetEmissiveColor().m128_f32[2], materials[i]->GetEmissiveColor().m128_f32[3] };
        ImVec4 specular = { materials[i]->GetSpecularColor().m128_f32[0], materials[i]->GetSpecularColor().m128_f32[1], materials[i]->GetSpecularColor().m128_f32[2], materials[i]->GetSpecularColor().m128_f32[3] };

        ImGui::SetNextItemAllowOverlap();
        if (ImGui::Selectable(materials[i]->GetName().c_str(), i == index))
            index = i;
        ImGui::SameLine();
        ImGui::ColorButton("diffuse", diffuse, ImGuiColorEditFlags_None, buttonSize);
        ImGui::SameLine();
        ImGui::ColorButton("emissive", emissive, ImGuiColorEditFlags_None, buttonSize);
        ImGui::SameLine();
        ImGui::ColorButton("specular", specular, ImGuiColorEditFlags_None, buttonSize);
    }
}

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
    if (ImGui::InputScalar("cull", ImGuiDataType_U32, &numCulled, &steps)) {
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
    if (ImGui::InputScalar("index count", ImGuiDataType_U32, &indexCount, &steps))
        submesh.SetIndexCount(indexCount);
    if (ImGui::InputScalar("start index", ImGuiDataType_U32, &startIndex, &steps))
        submesh.SetStartIndex(startIndex);
    if (ImGui::InputScalar("vertex offset", ImGuiDataType_U32, &vertexOffset, &steps))
        submesh.SetVertexOffset(vertexOffset);
    ImGui::PopItemWidth();
}

void DebugUI::ModelHierarchy(Scene& scene, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh) {
    int expandAll = -1;
    if (ImGui::Button("Expand all"))
        expandAll = 1;
    ImGui::SameLine();
    if (ImGui::Button("Collapse all"))
        expandAll = 0;

    static ImGuiTreeNodeFlags hierarchyBaseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    for (auto modelPair : scene.GetModels()) {
        ImGuiTreeNodeFlags hierarchyNodeFlags = hierarchyBaseNodeFlags;

        if (expandAll != -1)
            ImGui::SetNextItemOpen(expandAll != 0);
        bool modelNodeOpen = ImGui::TreeNodeEx(modelPair.first.c_str(), hierarchyNodeFlags);

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

void DebugUI::BoneHierarchy(Model& model, std::uint32_t& selectedBone) {
    ImGui::BeginChild("bone_hierarchy", ImVec2(ImGui::GetWindowWidth() - 15.f, 200.f), ImGuiChildFlags_None);
    for (std::uint32_t i = 0; i < model.GetNumBones(); ++i) {
        std::string text = std::to_string(i) + ": " + model.GetBones()[i].GetName() + "; " + std::to_string(model.GetBones()[i].GetParentIndex());
        if (ImGui::Selectable(text.c_str(), selectedBone == i))
            selectedBone = i;
    }
    ImGui::EndChild();
}

void DebugUI::CameraMenu(Camera& camera) {
    ImGuiViewport* pViewport = ImGui::GetMainViewport();
    static float z[2] = { camera.GetNearZ(), camera.GetFarZ() };
    static float windowSize[2] = { pViewport->WorkSize.x, pViewport->WorkSize.y };
    static float fovY = camera.GetFovY();

    bool isOrthographic = camera.IsOrthographic();
    ImGui::Checkbox("orthographic", &isOrthographic);

    if (ImGui::Button("Reset")) {
        z[0] = .1f; z[1] = 2000.f;
        windowSize[0] = pViewport->WorkSize.x; windowSize[1] = pViewport->WorkSize.y;
        fovY = DirectX::XM_PIDIV4;
    }

    ImGui::SeparatorText("Projection");
    ImGui::DragFloat2("z planes", z, 1.f, .01f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::Text("Fov x: %f", camera.GetFovX());
    ImGui::DragFloat("fov y", &fovY, DirectX::XMConvertToRadians(.5f), .01f, std::numeric_limits<float>::max());

    ImGui::SeparatorText("Window size");
    ImGui::Text("Aspect: %f", camera.GetAspect());
    ImGui::DragFloat2("window size", windowSize, 1.f, .1f, std::numeric_limits<float>::max(), "%.3f", ImGuiSliderFlags_AlwaysClamp);
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

void DebugUI::MaterialMenu(Material& material) {
    if (ImGui::TreeNode("Textures")) {
        MaterialTextures(material);
        ImGui::TreePop();
        ImGui::Spacing();
    }
    if (ImGui::TreeNode("RenderFlags")) {
        RenderFlags(material.GetFlags());
        ImGui::TreePop();
        ImGui::Spacing();
    }
    if (ImGui::TreeNode("Colors")) {
        MaterialColors(material);
        ImGui::TreePop();
        ImGui::Spacing();
    }
}


void DebugUI::BoneMenu(Model& model, std::uint32_t boneIndex) {
    std::vector<Bone>& bones = model.GetBones();
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

void DebugUI::SubmeshMenu(Submesh& submesh, Model& grandParent) {
    bool visible = submesh.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        submesh.SetVisible(visible);
    if (ImGui::CollapsingHeader("Available materials")) {
        std::uint32_t materialIndex = submesh.GetMaterialIndex();
        MaterialSelector(materialIndex, grandParent.GetMaterials());
        submesh.SetMaterialIndex(materialIndex);
    }
    if (ImGui::CollapsingHeader("Instancing"))
        SubmeshInstances(submesh);
    if (ImGui::CollapsingHeader("Vertex indexing"))
        SubmeshVertexIndexing(submesh);
}

void DebugUI::IMeshMenu(IMesh& iMesh) {
    bool visible = iMesh.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        iMesh.SetVisible(visible);
    if (auto pMesh = dynamic_cast<Mesh*>(&iMesh)) {
        if (ImGui::CollapsingHeader("Vertices")) 
            Vertices(pMesh->GetVertices());
    }
    if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(&iMesh)) {
        if (ImGui::CollapsingHeader("Vertices")) 
            Vertices(pSkinnedMesh->GetVertices());
    }
    if (ImGui::CollapsingHeader("Indices"))
        Indices(iMesh.GetIndices(), iMesh.GetNumVertices());
}

void DebugUI::ModelMenu(Model& model) {
    bool visible = model.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        model.SetVisible(visible);
    if (ImGui::CollapsingHeader("World transform")) {
        HelpMarker("Will apply transformation to all instances of all submeshes of all meshes in this model.\n!CAUTION! meshes could be shared beteen models.");
        DirectX::XMFLOAT3X4 W;
        if (AffineTransformation(W))
            model.SetWorldTransform(W);
    }
    if (ImGui::CollapsingHeader("Armature")) {
        static std::uint32_t selectedBone = std::uint32_t(-1);
        BoneHierarchy(model, selectedBone);
        if (selectedBone != std::uint32_t(-1) && selectedBone < model.GetNumBones()) {
            ImGui::SeparatorText(model.GetBones()[selectedBone].GetName().c_str());
            BoneMenu(model, selectedBone);
            ImGui::Separator();
        }
    }
    if (ImGui::CollapsingHeader("Materials")) {
        for (std::shared_ptr<Material>& pMaterial : model.GetMaterials()) {
            if (ImGui::TreeNode(pMaterial->GetName().c_str())) {
                MaterialMenu(*pMaterial);
                
                ImGui::TreePop();
                ImGui::Spacing();
            } 
        }
    }
}

void DebugUI::SpriteMenu(Sprite& sprite) {
    bool visible = sprite.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        sprite.SetVisible(visible);
    ImGui::Text("File: %ws", sprite.GetFilePath().c_str());
    if (ImGui::CollapsingHeader("Position")) {
        float origin[2] = { sprite.GetOrigin().x, sprite.GetOrigin().y };
        float offset[2] = { sprite.GetOffset().x, sprite.GetOffset().y };
        float scale[2] = { sprite.GetScale().x, sprite.GetScale().y };
        float layer = sprite.GetLayer();
        float angle = DirectX::XMConvertToDegrees(sprite.GetAngle());

        if (ImGui::DragFloat2(std::string("origin##" + sprite.GetName()).c_str(), origin))
            sprite.GetOrigin() = { origin[0], origin[1] };
        if (ImGui::DragFloat2(std::string("offset##" + sprite.GetName()).c_str(), offset))
            sprite.GetOffset() = { offset[0], offset[1] };
        if (ImGui::DragFloat2(std::string("scale##" + sprite.GetName()).c_str(), scale))
            sprite.GetScale() = { scale[0], scale[1] };
        if (ImGui::DragFloat(std::string("layer##" + sprite.GetName()).c_str(), &layer))
            sprite.SetLayer(layer);
        if (ImGui::DragFloat(std::string("angle##" + sprite.GetName()).c_str(), &angle))
            sprite.SetAngle(DirectX::XMConvertToRadians(angle));
    }
    if (ImGui::CollapsingHeader("Color"))
        ImGui::ColorEdit4(std::string("color##" + sprite.GetName()).c_str(), sprite.GetColor().m128_f32);
}

void DebugUI::IOutlineMenu(IOutline& outline) {
    bool visible = outline.IsVisible();
    if (ImGui::Checkbox("Visible", &visible))
        outline.SetVisible(visible);
    if (ImGui::CollapsingHeader("Position")) {
        if (auto p = dynamic_cast<BoundingBodyOutline<DirectX::BoundingBox>*>(&outline)) {
            DirectX::BoundingBox& b = p->GetBounds();

            static float center[3] = { b.Center.x, b.Center.y, b.Center.z };
            static float extents[3] = { b.Extents.x, b.Extents.y, b.Extents.z };

            if (ImGui::DragFloat3(std::string("center##" + outline.GetName()).c_str(), center))
                b.Center = { center[0], center[1], center[2] };
            if (ImGui::DragFloat3(std::string("extents##" + outline.GetName()).c_str(), extents))
                b.Extents = { extents[0], extents[1], extents[2] };
        } else if (auto p = dynamic_cast<BoundingBodyOutline<DirectX::BoundingFrustum>*>(&outline)) {
            DirectX::BoundingFrustum& b = p->GetBounds();

            static float origin[3] = { b.Origin.x, b.Origin.y, b.Origin.z };

            DirectX::SimpleMath::Quaternion q = b.Orientation;
            DirectX::XMFLOAT3 r = q.ToEuler();
            static float rotation[3] = { r.x, r.y, r.z };

            if (ImGui::DragFloat3(std::string("origin##" + outline.GetName()).c_str(), origin))
                b.Origin = { origin[0], origin[1], origin[2] };
            if (ImGui::DragFloat3(std::string("rotation##" + outline.GetName()).c_str(), rotation))
                DirectX::XMStoreFloat4(&b.Orientation, DirectX::XMQuaternionRotationRollPitchYaw(
                            DirectX::XMConvertToRadians(rotation[0]), 
                            DirectX::XMConvertToRadians(rotation[1]), 
                            DirectX::XMConvertToRadians(rotation[2])));
            ImGui::DragFloat(std::string("right slope##" + outline.GetName()).c_str(), &b.RightSlope);
            ImGui::DragFloat(std::string("left slope##" + outline.GetName()).c_str(), &b.LeftSlope);
            ImGui::DragFloat(std::string("top slope##" + outline.GetName()).c_str(), &b.TopSlope);
            ImGui::DragFloat(std::string("bottom slope##" + outline.GetName()).c_str(), &b.BottomSlope);
            ImGui::DragFloat(std::string("near##" + outline.GetName()).c_str(), &b.Near);
            ImGui::DragFloat(std::string("far##" + outline.GetName()).c_str(), &b.Far);
        } else if (auto p = dynamic_cast<BoundingBodyOutline<DirectX::BoundingOrientedBox>*>(&outline)) {
            DirectX::BoundingOrientedBox& b = p->GetBounds();

            static float center[3] = { b.Center.x, b.Center.y, b.Center.z };
            static float extents[3] = { b.Extents.x, b.Extents.y, b.Extents.z };
            DirectX::SimpleMath::Quaternion q = b.Orientation;
            DirectX::XMFLOAT3 r = q.ToEuler();
            static float rotation[3] = { r.x, r.y, r.z };

            if (ImGui::DragFloat3(std::string("center##" + outline.GetName()).c_str(), center))
                b.Center = { center[0], center[1], center[2] };
            if (ImGui::DragFloat3(std::string("extents##" + outline.GetName()).c_str(), extents))
                b.Extents = { extents[0], extents[1], extents[2] };
            if (ImGui::DragFloat3(std::string("rotation##" + outline.GetName()).c_str(), rotation))
                DirectX::XMStoreFloat4(&b.Orientation, DirectX::XMQuaternionRotationRollPitchYaw(
                            DirectX::XMConvertToRadians(rotation[0]), 
                            DirectX::XMConvertToRadians(rotation[1]), 
                            DirectX::XMConvertToRadians(rotation[2])));
        } else if (auto p = dynamic_cast<BoundingBodyOutline<DirectX::BoundingSphere>*>(&outline)) {
            DirectX::BoundingSphere& b = p->GetBounds();

            static float center[3] = { b.Center.x, b.Center.y, b.Center.z };

            if (ImGui::DragFloat3(std::string("center##" + outline.GetName()).c_str(), center))
                b.Center = { center[0], center[1], center[2] };
            ImGui::DragFloat(std::string("radius##" + outline.GetName()).c_str(), &b.Radius);
        } else if (auto p = dynamic_cast<GridOutline*>(&outline)) {
            static std::uint16_t xDiv = p->GetXDivsions();
            static std::uint16_t yDiv = p->GetYDivsions();

            if (ImGui::DragScalar(std::string("x divisions##" + outline.GetName()).c_str(), ImGuiDataType_U16, &xDiv))
                p->SetXDivisions(xDiv);
            if (ImGui::DragScalar(std::string("y divisions##" + outline.GetName()).c_str(), ImGuiDataType_U16, &yDiv))
                p->SetYDivisions(yDiv);
            ImGui::DragFloat3(std::string("x axis##" + outline.GetName()).c_str(), p->GetXAxis().m128_f32);
            ImGui::DragFloat3(std::string("y axis##" + outline.GetName()).c_str(), p->GetYAxis().m128_f32);
            ImGui::DragFloat3(std::string("origin##" + outline.GetName()).c_str(), p->GetOrigin().m128_f32);
        } else if (auto p = dynamic_cast<RingOutline*>(&outline)) {
            ImGui::DragFloat3(std::string("minor axis##" + outline.GetName()).c_str(), p->GetMajorAxis().m128_f32);
            ImGui::DragFloat3(std::string("major axis##" + outline.GetName()).c_str(), p->GetMinorAxis().m128_f32);
            ImGui::DragFloat3(std::string("origin##" + outline.GetName()).c_str(), p->GetOrigin().m128_f32);
        } else if (auto p = dynamic_cast<RayOutline*>(&outline)) {
            bool normalized = p->IsNormalized();
            if (ImGui::Checkbox("Normalized", &normalized))
                p->SetNormalized(normalized);
            ImGui::DragFloat3(std::string("direction##" + outline.GetName()).c_str(), p->GetDirection().m128_f32);
            ImGui::DragFloat3(std::string("origin##" + outline.GetName()).c_str(), p->GetOrigin().m128_f32);
        } else if (auto p = dynamic_cast<TriangleOutline*>(&outline)) {
            ImGui::DragFloat3(std::string("point A##" + outline.GetName()).c_str(), p->GetPointA().m128_f32);
            ImGui::DragFloat3(std::string("point B##" + outline.GetName()).c_str(), p->GetPointB().m128_f32);
            ImGui::DragFloat3(std::string("point C##" + outline.GetName()).c_str(), p->GetPointC().m128_f32);
        } else if (auto p = dynamic_cast<QuadOutline*>(&outline)) {
            ImGui::DragFloat3(std::string("point A##" + outline.GetName()).c_str(), p->GetPointA().m128_f32);
            ImGui::DragFloat3(std::string("point B##" + outline.GetName()).c_str(), p->GetPointB().m128_f32);
            ImGui::DragFloat3(std::string("point C##" + outline.GetName()).c_str(), p->GetPointC().m128_f32);
            ImGui::DragFloat3(std::string("point D##" + outline.GetName()).c_str(), p->GetPointD().m128_f32);
        }
    }
    if (ImGui::CollapsingHeader("Color"))
        ImGui::ColorEdit4(std::string("color##" + outline.GetName()).c_str(), outline.GetColor().m128_f32);

}

void DebugUI::SceneWindow(Scene& scene, ImGuiWindowFlags windowFlags) {
    int ID = 0;

    IMGUI_CHECKVERSION();

    static bool open = true;
    if (!ImGui::Begin("Scene", &open, windowFlags)) {
        ImGui::End();
        return;
    }

    static Model* pSelectedModel = nullptr;
    static IMesh* pSelectedIMesh = nullptr;
    static Submesh* pSelectedSubmesh = nullptr;

    if (ImGui::CollapsingHeader("Stats")) {
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

        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Camera")) {
        CameraMenu(scene.GetCamera());
        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Models")) {
        ImGui::BeginChild(ImGui::GetID((void*)(intptr_t)ID++), ImVec2(ImGui::GetWindowWidth() - 15.f, 200.f), ImGuiChildFlags_None);
        ModelHierarchy(scene, &pSelectedModel, &pSelectedIMesh, &pSelectedSubmesh);
        ImGui::EndChild();

        ImGui::Separator();

        ImGui::Text("%s > %s > %s", 
                pSelectedModel ? pSelectedModel->GetName().c_str() : "...", 
                pSelectedIMesh ? pSelectedIMesh->GetName().c_str() : "...",
                pSelectedSubmesh ? pSelectedSubmesh->GetName().c_str() : "...");
        ImGui::SameLine();
        HelpMarker("Model > Mesh > Submesh");

        if (pSelectedSubmesh)
            SubmeshMenu(*pSelectedSubmesh, *pSelectedModel);
        else if (pSelectedIMesh)
            IMeshMenu(*pSelectedIMesh);
        else if (pSelectedModel)
            ModelMenu(*pSelectedModel);

        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Sprites")) {
        for (auto& spritePair : scene.GetSprites()) {
            if (ImGui::TreeNode(spritePair.first.c_str())) {
                SpriteMenu(*spritePair.second);
                ImGui::TreePop();
            }
        }
        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Text")) {
        for (auto& textPair : scene.GetTexts()) {
            if (ImGui::TreeNode(textPair.first.c_str())) {
                SpriteMenu(*textPair.second);
                ImGui::TreePop();
            }
        }
        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Outlines")) {
        for (auto& outlinePair : scene.GetOutlines()) {
            if (ImGui::TreeNode(outlinePair.first.c_str())) {
                IOutlineMenu(*outlinePair.second);
                ImGui::TreePop();
            }
        }

        ImGui::Separator();
        ImGui::Spacing();
    }

    ImGui::End();
}

