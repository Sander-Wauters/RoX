#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Model.h>
#include <RoX/MeshFactory.h>

namespace MeshFactoryUI {
    void GeoSelector(MeshFactory::Geometry& geo);

    void AddCubeToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddBoxToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddSphereToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddGeoSphereToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddCylinderToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddConeToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddTorusToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddTetrahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddOctahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddDodecahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddIcosahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddTeapotToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);

    void AddGeoToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
}

