#pragma once

#include <RoX/AssetBatch.h>

namespace AssetBatchUI {
    void Selector(std::uint8_t& index, std::vector<std::shared_ptr<AssetBatch>>& batches);

    void Stats(AssetBatch& batch);

    void Menu(AssetBatch& batch);
}

