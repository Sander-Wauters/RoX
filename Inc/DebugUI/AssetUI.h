#pragma once

#include <RoX/Asset.h>
#include <RoX/AssetBatch.h>

namespace AssetUI {
    void Remover(AssetBatch::AssetType type, AssetBatch& batch);

    void Menu(Asset& asset);

    void RemoverPopupMenu(AssetBatch::AssetType type, AssetBatch& batch);
}
