#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Model.h>

namespace BoneUI {
    void Selector(std::uint32_t& index, std::vector<Bone>& bones);

    void Menu(Model& model, std::uint32_t boneIndex);
}

