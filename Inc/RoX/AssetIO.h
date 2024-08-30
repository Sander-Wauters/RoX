#pragma once

#include <unordered_map>

#include "Model.h"
#include "Animation.h"

// Hold function for dealing with the importing and exporting of assets.
namespace AssetIO {
    // Imports the meshes and bones from a file.
    // Uses **assimp** and should only be used to import from source files.
    // Should not be used in production code due to the high parsing costs.
    std::shared_ptr<Model> ImportModel(std::string filepath, std::shared_ptr<Material> material, bool skinned, bool packMeshes);

    // Imports all animations from a file.
    // Uses **assimp** and should only be used to import from source files.
    // Should not be used in production code due to the high parsing costs.
    std::unordered_map<std::string, std::shared_ptr<Animation>> ImportAnimations(std::string filepath);
};
