#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Model.h>

namespace ModelUI {
    void Selector(AssetBatch& batch, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh);

    void Creator(AssetBatch& batch);

    void Menu(Model& model, const Materials& availableMaterials);

    void CreatorPopupMenu(AssetBatch& batch);
}

