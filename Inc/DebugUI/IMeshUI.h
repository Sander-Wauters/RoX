#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Model.h>

namespace IMeshUI {
    void Selector(std::uint32_t& index, std::vector<std::shared_ptr<IMesh>>& meshes);

    void Creator(Model& model);
    void Remover(Model& model);

    void Menu(IMesh& iMesh);

    void CreatorPopupMenu(Model& model);
    void AddGeoOrSubmeshPopupMenu(AssetBatch& batch, Model& model, IMesh& iMesh);
    void RemoverPopupMenu(Model& model);
}

