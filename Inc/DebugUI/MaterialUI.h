#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Material.h>
#include <RoX/Model.h>

namespace MaterialUI {
    void RenderFlagsPresets(std::uint32_t& renderFlags);
    void RenderFlagsBlendState(std::uint32_t& renderFlags);
    void RenderFlagsDepthStencilState(std::uint32_t& renderFlags);
    void RenderFlagsRasterizerState(std::uint32_t& renderFlags);
    void RenderFlagsSamplerState(std::uint32_t& renderFlags);
    void RenderFlagsEffects(std::uint32_t& renderFlags);
    void RenderFlags(std::uint32_t& renderFlags);

    void Header(Material& material);
    bool SelectableHeader(bool state, Material& material);
    bool TreeNodeHeader(Material& material);

    void Selector(std::uint32_t& index, const std::vector<std::shared_ptr<Material>>& materials);
    void Selector(std::uint64_t& GUID, const Materials& materials);

    void Textures(Material& material);
    void Colors(Material& material);

    void Creator(AssetBatch& batch);
    void Remover(Model& model);

    void Menu(Material& material);
    void Menu(std::vector<std::shared_ptr<Material>>& materials);
    void Menu(const std::unordered_map<std::uint64_t, std::shared_ptr<Material>>& materials);

    void CreatorPopupMenu(AssetBatch& batch);
    void AdderPopupMenu(Model& model, const Materials& availableMaterials);
    void RemoverPopupMenu(Model& model);
}

