#pragma once

#include <unordered_map>

#include "Model.h"
#include "Animation.h"

namespace AssetIO {
    std::shared_ptr<Model> ImportModel(std::string filepath, std::shared_ptr<Material> material, bool skinned, bool packMeshes);
    std::unordered_map<std::string, std::shared_ptr<Animation>> ImportAnimations(std::string filepath);
};
