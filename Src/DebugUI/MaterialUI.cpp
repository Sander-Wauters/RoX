#include "DebugUI/MaterialUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/LightingUI.h"
#include "DebugUI/Util.h"
#include "DebugUI/GeneralUI.h"
#include "DebugUI/AssetUI.h"

void MaterialUI::RenderFlagsPresets(std::uint32_t& renderFlags) {
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

void MaterialUI::RenderFlagsBlendState(std::uint32_t& renderFlags) {
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

void MaterialUI::RenderFlagsDepthStencilState(std::uint32_t& renderFlags) {
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

void MaterialUI::RenderFlagsRasterizerState(std::uint32_t& renderFlags) {
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

void MaterialUI::RenderFlagsSamplerState(std::uint32_t& renderFlags) {
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

void MaterialUI::RenderFlagsEffects(std::uint32_t& renderFlags) {
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

void MaterialUI::RenderFlags(std::uint32_t& renderFlags) {
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

void MaterialUI::Header(Material& material) {
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

bool MaterialUI::SelectableHeader(bool state, Material& material) {
    bool selected = state;
    ImVec2 buttonSize(ImGui::GetFontSize(), ImGui::GetFontSize());
    ImVec4 diffuse  = { material.GetDiffuseColor().x,  material.GetDiffuseColor().y,  material.GetDiffuseColor().z,  material.GetDiffuseColor().w  };
    ImVec4 emissive = { material.GetEmissiveColor().x, material.GetEmissiveColor().y, material.GetEmissiveColor().z, material.GetEmissiveColor().w }; 
    ImVec4 specular = { material.GetSpecularColor().x, material.GetSpecularColor().y, material.GetSpecularColor().z, material.GetSpecularColor().w };

    ImGui::SetNextItemAllowOverlap();
    if (ImGui::Selectable(material.GetName().c_str(), state, ImGuiSelectableFlags_DontClosePopups))
        selected = true;
    ImGui::SameLine();
    ImGui::ColorButton("diffuse", diffuse, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("emissive", emissive, ImGuiColorEditFlags_None, buttonSize);
    ImGui::SameLine();
    ImGui::ColorButton("specular", specular, ImGuiColorEditFlags_None, buttonSize);
    return selected;
}

bool MaterialUI::TreeNodeHeader(Material& material) {
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

void MaterialUI::Selector(std::uint32_t& index, const std::vector<std::shared_ptr<Material>>& materials) {
    for (std::uint32_t i = 0; i < materials.size(); ++i) {
        if (SelectableHeader(index == i, *materials[i]))
            index = i;
    }
}

void MaterialUI::Selector(std::uint64_t& GUID, const Materials& materials) {
    for (auto& materialPair : materials) {
        if (SelectableHeader(materialPair.first == GUID, *materialPair.second))
            GUID = materialPair.first;
    }
}

void MaterialUI::Textures(Material& material) {
    ImGui::Text("Diffuse map: %ws", material.GetDiffuseMapFilePath().c_str());
    ImGui::Text("Normal map:  %ws", material.GetNormalMapFilePath().c_str());
}

void MaterialUI::Colors(Material& material) {
    float diffuse[4];
    Util::StoreFloat4(material.GetDiffuseColor(), diffuse);
    float emissive[4];
    Util::StoreFloat4(material.GetEmissiveColor(), emissive);
    float specular[4];
    Util::StoreFloat4(material.GetSpecularColor(), specular);

    if (ImGui::ColorEdit4(Util::GUIDLabel("Diffuse", material.GetGUID()).c_str(), diffuse))
        Util::LoadFloat4(diffuse, material.GetDiffuseColor());
    if (ImGui::ColorEdit4(Util::GUIDLabel("Emissive", material.GetGUID()).c_str(), emissive))
        Util::LoadFloat4(emissive, material.GetEmissiveColor());
    if (ImGui::ColorEdit4(Util::GUIDLabel("Specular", material.GetGUID()).c_str(), specular))
        Util::LoadFloat4(specular, material.GetSpecularColor());
}

void MaterialUI::Lights(Material& material) {
    float ambient[3];
    Util::StoreFloat3(material.GetAmbientLight(), ambient);

    if (ImGui::ColorEdit3(Util::GUIDLabel("Ambient", material.GetGUID()).c_str(), ambient))
        Util::LoadFloat3(ambient, material.GetAmbientLight());

    ImGui::SeparatorText("Directional lights");

    if (ImGui::Button("Add new directional light##Lights"))
        material.GetDirectionalLights().push_back(std::make_shared<DirectionalLight>());
    
    for (std::uint8_t i = 0; i < material.GetNumDirectionalLights() && i < Material::MAX_DIRECTIONAL_LIGHTS; ++i) {
        if (ImGui::TreeNode(Util::GUIDLabel(std::to_string(i), material.GetGUID()).c_str())) {
            LightingUI::DirectionalLightMenu(*material.GetDirectionalLights()[i]);
            ImGui::TreePop();
        }
    }
}

void MaterialUI::Creator(AssetBatch& batch) {
    static char diffuseMapFilePath[128] = "";
    static char normalMapFilePath[128] = "";
    static char name[128] = "";
    static std::uint32_t renderFlags = RenderFlags::None;
    static float diffuse[4] = { 1.f, 1.f, 1.f, 1.f };
    static float emissive[4] = { 0.f, 0.f, 0.f, 1.f };
    static float specular[4] = { 1.f, 1.f, 1.f, 1.f };

    ImGui::InputText("Name##MaterialCreator", name, std::size(name));

    ImGui::SeparatorText("Textures");
    bool validDiffuse = GeneralUI::InputFilePath("Diffuse map##MaterialCreator", diffuseMapFilePath, std::size(diffuseMapFilePath));
    GeneralUI::SameLineError(!validDiffuse, "file not found");
    bool validNormal = GeneralUI::InputFilePath("Normal map##MaterialCreator", normalMapFilePath, std::size(normalMapFilePath));
    GeneralUI::SameLineError(!validNormal, "file not found");

    ImGui::SeparatorText("Render flags");
    RenderFlags(renderFlags);

    ImGui::SeparatorText("Colors");

    ImGui::ColorEdit4("Diffuse##MaterialCreator", diffuse);
    ImGui::ColorEdit4("Emissive##MaterialCreator", emissive);
    ImGui::ColorEdit4("Specular##MaterialCreator", specular);

    if (ImGui::Button("Create new material##MaterialCreator") && validDiffuse && validNormal) {
        UpdateScheduler::Get().Add([&](){
            DirectX::XMFLOAT4 D;
            Util::LoadFloat4(diffuse, D);
            DirectX::XMFLOAT4 E;
            Util::LoadFloat4(emissive, E);
            DirectX::XMFLOAT4 S;
            Util::LoadFloat4(specular, S);

            batch.Add(std::make_shared<Material>(
                        diffuseMapFilePath,
                        normalMapFilePath,
                        std::string(name),
                        renderFlags,
                        DirectX::XMLoadFloat4(&D), 
                        DirectX::XMLoadFloat4(&E),
                        DirectX::XMLoadFloat4(&S)));
        });
    }
}

void MaterialUI::Remover(Model& model) {
    std::uint32_t selected = std::uint32_t(-1);
    Selector(selected, model.GetMaterials());
    if (selected != std::uint32_t(-1))
        UpdateScheduler::Get().Add([&, selected](){ model.RemoveMaterial(selected); });
}

void MaterialUI::Menu(Material& material) {
    std::uint32_t renderFlags = material.GetFlags();

    ImGui::SeparatorText("Identifiers");
    AssetUI::Menu(material);

    ImGui::SeparatorText("Textures");
    Textures(material);

    ImGui::SeparatorText("Render flags");
    RenderFlags(renderFlags);

    ImGui::SeparatorText("Colors");
    Colors(material);

    ImGui::SeparatorText("Lights");
    Lights(material);
}

void MaterialUI::Menu(std::vector<std::shared_ptr<Material>>& materials) {
    for (std::shared_ptr<Material>& pMaterial : materials) {
        if (TreeNodeHeader(*pMaterial)) {
            Menu(*pMaterial);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }
}

void MaterialUI::Menu(const std::unordered_map<std::uint64_t, std::shared_ptr<Material>>& materials) {
    for (auto& materialPair : materials) {
        if (TreeNodeHeader(*materialPair.second)) {
            Menu(*materialPair.second);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }
}

void MaterialUI::CreatorPopupMenu(AssetBatch& batch) {
    if (ImGui::Button(Util::GUIDLabel("+", "MaterialCreatorPopupMenu").c_str())) 
        ImGui::OpenPopup("MaterialCreatorPopupMenu");
    if (ImGui::BeginPopup("MaterialCreatorPopupMenu")) {
        Creator(batch);
        ImGui::EndPopup();
    }
}

void MaterialUI::AdderPopupMenu(Model& model, const Materials& availableMaterials) {
    if (ImGui::Button(Util::GUIDLabel("+", "MaterialAdderPopupMenu").c_str())) 
        ImGui::OpenPopup("MaterialAdderPopupMenu");
    if (ImGui::BeginPopup("MaterialAdderPopupMenu")) {
        std::uint64_t selected = Asset::INVALID_GUID;
        Selector(selected, availableMaterials);
        if (selected != Asset::INVALID_GUID)
            UpdateScheduler::Get().Add([&, selected](){ model.Add(availableMaterials.at(selected)); });
        ImGui::EndPopup();
    }
}

void MaterialUI::RemoverPopupMenu(Model& model) {
    if (ImGui::Button(Util::GUIDLabel("-", "MaterialRemoverPopupMenu").c_str())) 
        ImGui::OpenPopup("MaterialRemoverPopupMenu");
    if (ImGui::BeginPopup("MaterialRemoverPopupMenu")) {
        Remover(model);
        ImGui::EndPopup();
    }
}

