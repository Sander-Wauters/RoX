#pragma once

#include "MaterialDeviceData.h"
#include "MeshDeviceData.h"

class IDeviceDataSupplier {
    public:
        virtual std::shared_ptr<MaterialDeviceData> GetMaterialDeviceData(const std::shared_ptr<Material>& pMaterial) = 0;
        virtual std::shared_ptr<MeshDeviceData> GetMeshDeviceData(const std::shared_ptr<IMesh>& pIMesh) = 0;

        virtual void SignalMeshRemoved() = 0;
};
