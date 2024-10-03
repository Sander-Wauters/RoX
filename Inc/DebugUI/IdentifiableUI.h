#pragma once

#include <RoX/Identifiable.h>
#include <RoX/AssetBatch.h>

namespace IdentifiableUI {
    void Remover(AssetBatch::AssetType type, AssetBatch& batch);

    void Menu(Identifiable& asset);

    void RemoverPopupMenu(AssetBatch::AssetType type, AssetBatch& batch);
}

