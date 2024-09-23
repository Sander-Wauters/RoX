#pragma once

#include <RoX/Model.h>

#include "ValidMesh.h"
#include "ValidMaterial.h"

class ValidModel : public ValidMaterial, public ValidMesh {
    public:
        ValidModel();

        std::shared_ptr<Model> NewValidModelWithNewValidMaterial();
        std::shared_ptr<Model> NewValidModelWithExistingMaterial();
        std::shared_ptr<Model> NewValidSkinnedModelWithNewValidMaterial();
        std::shared_ptr<Model> NewValidSkinnedModelWithExistingMaterial();

        std::shared_ptr<Model> pModel;
        std::shared_ptr<Model> pSkinnedModel;

        std::uint64_t ModelGUID;
        std::uint64_t SkinnedModelGUID;

        std::string ModelName;
        std::string SkinnedModelName;
};
