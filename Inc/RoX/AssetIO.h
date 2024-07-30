#pragma once

#include "Model.h"

namespace AssetIO {
    std::shared_ptr<Model> ImportModel(std::string filepath, std::shared_ptr<Material> material, bool skinned, bool packMeshes);
};
