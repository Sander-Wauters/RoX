#include "RoX/DebugUI.h"

#undef max
#include "ImGui/imgui.h"

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
    //DirectX::XMVECTOR indices = DirectX::XMLoadUInt4(&vertex.indices);
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
    ImGui::DragFloat4("r0", matrix.m[0], 0.25f);
    ImGui::SameLine();
    HelpMarker("matrix column-major");
    ImGui::DragFloat4("r1", matrix.m[1], 0.25f);
    ImGui::DragFloat4("r2", matrix.m[2], 0.25f);
    ImGui::InputFloat4("r3", r3, "%.3f", ImGuiInputTextFlags_ReadOnly);
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

void DebugUI::MaterialEditor(Material& material) {
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

    if (index >= 0 && index < submesh.GetNumInstances())
        Matrix(submesh.GetInstances()[index]);
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

void DebugUI::BoneHierarchy(std::vector<Bone>& bones, Bone** ppSelectedBone, std::uint32_t index) {
    static ImGuiTreeNodeFlags hierarchyBaseNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (bones.size() == 0)
        return;

    ImGuiTreeNodeFlags hierarchyNodeFlags = hierarchyBaseNodeFlags;
    Bone& bone = bones[index]; 

    if (bone.IsLeaf()) {
        hierarchyNodeFlags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(bone.GetName().c_str(), hierarchyNodeFlags);
        return;
    }

    if (ImGui::TreeNode(bone.GetName().c_str())) {
        for (std::uint32_t i = 0; i < bone.GetNumChildren(); ++i) {
            BoneHierarchy(bones, ppSelectedBone, bone.GetChildIndices()[i]);
        }
        ImGui::TreePop();
    }
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
    ImGui::DragFloat2("z planes", z);
    ImGui::Text("Fov x: %f", camera.GetFovX());
    ImGui::DragFloat("fov y", &fovY, DirectX::XMConvertToRadians(.5f));

    ImGui::SeparatorText("Window size");
    ImGui::Text("Aspect: %f", camera.GetAspect());
    ImGui::DragFloat2("window size", windowSize);
    ImGui::Text("Near plane width:  %f", camera.GetNearWindowWidth());
    ImGui::Text("Near plane height: %f", camera.GetNearWindowHeight());
    ImGui::Spacing();
    ImGui::Text("Far plane width:   %f", camera.GetFarWindowWidth());
    ImGui::Text("Far plane height:  %f", camera.GetFarWindowHeight());

    z[0] = std::max(z[0], .1f); z[1] = std::max(z[1], .1f);
    fovY = std::max(fovY, .1f);

    if (isOrthographic)
        camera.SetOrthographicView(windowSize[0], windowSize[1], z[0], z[1]);
    else
        camera.SetPerspectiveView(fovY, windowSize[0] / windowSize[1], z[0], z[1]);
    camera.Update();
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
    if(ImGui::CollapsingHeader("Vertex indexing"))
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

    if (ImGui::CollapsingHeader("Materials")) {
        for (std::shared_ptr<Material>& pMaterial : model.GetMaterials()) {
            if (ImGui::TreeNode(pMaterial->GetName().c_str())) {
                MaterialEditor(*pMaterial);
                
                ImGui::TreePop();
                ImGui::Spacing();
            } 
        }
    }
    if (ImGui::CollapsingHeader("Armature"))
        BoneHierarchy(model.GetBones(), nullptr);
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

        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Text")) {

        ImGui::Separator();
        ImGui::Spacing();
    }
    if (ImGui::CollapsingHeader("Outlines")) {

        ImGui::Separator();
        ImGui::Spacing();
    }

    ImGui::End();
}

