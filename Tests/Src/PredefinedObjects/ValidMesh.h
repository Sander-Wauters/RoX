#pragma once

#include <RoX/Model.h>

class ValidMesh {
    public:
        ValidMesh();

        std::unique_ptr<Submesh>     NewValidSubmesh();
        std::shared_ptr<Mesh>        NewValidMesh();
        std::shared_ptr<SkinnedMesh> NewValidSkinnedMesh();

        std::shared_ptr<Mesh> pMesh;
        std::shared_ptr<SkinnedMesh> pSkinnedMesh;
};
