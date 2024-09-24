#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Model.h>

namespace SubmeshUI {
    void Instances(Submesh& submesh);
    void VertexIndexing(Submesh& submesh);

    void Selector(std::uint32_t& index, const std::vector<std::unique_ptr<Submesh>>& submeshes);

    void Creator(IMesh& iMesh, std::vector<std::shared_ptr<Material>>& availableMaterials);
    void Remover(IMesh& iMesh);

    void Menu(Submesh& submesh, std::vector<std::shared_ptr<Material>>& availableMaterials);

    void RemoverPopupMenu(IMesh& iMesh);
}

