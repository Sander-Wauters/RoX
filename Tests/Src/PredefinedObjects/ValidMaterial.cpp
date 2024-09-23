#include "ValidMaterial.h"

ValidMaterial::ValidMaterial() {
    pMaterial = NewValidMaterial();
    MaterialGUID = pMaterial->GetGUID();
    MaterialName = pMaterial->GetName();
}

std::shared_ptr<Material> ValidMaterial::NewValidMaterial() {
    return std::make_shared<Material>(TEXTURE_FILE_PATH, TEXTURE_FILE_PATH);
}
