#include "ValidModel.h"

ValidModel::ValidModel() {
    pModel = NewValidModelWithExistingMaterial();
    pSkinnedModel = NewValidSkinnedModelWithExistingMaterial();

    ModelGUID = pModel->GetGUID();
    SkinnedModelGUID = pSkinnedMesh->GetGUID();

    ModelName = pModel->GetName();
    SkinnedModelName = pSkinnedModel->GetName();

}

std::shared_ptr<Model> ValidModel::NewValidModelWithNewValidMaterial() {
    auto pModel = std::make_shared<Model>(NewValidMaterial());
    pModel->Add(pMesh);
    return pModel;
}

std::shared_ptr<Model> ValidModel::NewValidModelWithExistingMaterial() {
    auto pModel = std::make_shared<Model>(pMaterial);
    pModel->Add(pMesh);
    return pModel;
}

std::shared_ptr<Model> ValidModel::NewValidSkinnedModelWithNewValidMaterial() {
    auto pSkinnedModel = std::make_shared<Model>(NewValidMaterial());
    pSkinnedModel->Add(pSkinnedMesh);
    return pSkinnedModel;
}

std::shared_ptr<Model> ValidModel::NewValidSkinnedModelWithExistingMaterial() {
    auto pSkinnedModel = std::make_shared<Model>(pMaterial);
    pSkinnedModel->Add(pSkinnedMesh);
    return pSkinnedModel;
}

