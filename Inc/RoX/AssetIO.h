#pragma once

#include <unordered_map>

#include "Model.h"
#include "Animation.h"

// Hold function for dealing with the importing and exporting of assets.
namespace AssetIO {
    // Imports the meshes and bones from a file.
    // Uses **assimp** and should only be used to import from source files.
    // Should not be used in production code due to the high parsing costs.
    std::shared_ptr<Model> ImportModel(std::string filePath, std::shared_ptr<Material> material, bool skinned, bool packMeshes);

    // Imports all animations from a file.
    // Uses **assimp** and should only be used to import from source files.
    // Should not be used in production code due to the high parsing costs.
    std::unordered_map<std::string, std::shared_ptr<Animation>> ImportAnimations(std::string filePath);

    // Imports a model from a .roxmodl file.
    std::shared_ptr<Model> ImportRoXModl(std::string filePath, std::shared_ptr<Material> pMaterial);
    // Exports a model object into an .roxmodl file.
    void ExportRoXModl(std::shared_ptr<Model>& pModel, std::string filePath);

    // Imports an animation from a .roxanim file.
    std::shared_ptr<Animation> ImportRoXAnim(std::string filePath);
    // Exports an animation object into an .roxanim file.
    void ExportRoXAnim(std::shared_ptr<Animation>& pAnim, std::string filePath);
};
