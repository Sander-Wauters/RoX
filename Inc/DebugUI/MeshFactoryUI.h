#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Model.h>
#include <RoX/MeshFactory.h>

namespace MeshFactoryUI {
    void GeoSelector(MeshFactory::Geometry& geo);

    void AddCubeToIMeshCreator(IMesh& iMesh);
    void AddBoxToIMeshCreator(IMesh& iMesh);
    void AddSphereToIMeshCreator(IMesh& iMesh);
    void AddGeoSphereToIMeshCreator(IMesh& iMesh);
    void AddCylinderToIMeshCreator(IMesh& iMesh);
    void AddConeToIMeshCreator(IMesh& iMesh);
    void AddTorusToIMeshCreator(IMesh& iMesh);
    void AddTetrahedronToIMeshCreator(IMesh& iMesh);
    void AddOctahedronToIMeshCreator(IMesh& iMesh);
    void AddDodecahedronToIMeshCreator(IMesh& iMesh);
    void AddIcosahedronToIMeshCreator(IMesh& iMesh);
    void AddTeapotToIMeshCreator(IMesh& iMesh);

    void AddGeoToIMeshCreator(IMesh& iMesh);
}

