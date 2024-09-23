#include "ValidMesh.h"

ValidMesh::ValidMesh() {
    pMesh = NewValidMesh();
    pSkinnedMesh = NewValidSkinnedMesh();
}

std::unique_ptr<Submesh> ValidMesh::NewValidSubmesh() {
    return std::make_unique<Submesh>();
}

std::shared_ptr<Mesh> ValidMesh::NewValidMesh() {
    auto pMesh = std::make_shared<Mesh>();
    pMesh->Add(NewValidSubmesh());
    pMesh->GetVertices().push_back(VertexPositionNormalTexture());
    pMesh->GetIndices().push_back(0);

    return pMesh;
}

std::shared_ptr<SkinnedMesh> ValidMesh::NewValidSkinnedMesh() {
    auto pSkinnedMesh = std::make_shared<SkinnedMesh>();
    pSkinnedMesh->Add(NewValidSubmesh());
    pSkinnedMesh->GetVertices().push_back(VertexPositionNormalTextureSkinning());
    pSkinnedMesh->GetIndices().push_back(0);

    return pSkinnedMesh;
}

