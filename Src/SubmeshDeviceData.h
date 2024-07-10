#pragma once

#include <GraphicsMemory.h>
#include <Effects.h>
#include <ResourceUploadBatch.h>

#include "RoX/Model.h"

#include "Util/pch.h"

class SubmeshDeviceData {
    public:
        SubmeshDeviceData(ID3D12Device* pDevice, Submesh* pSubmesh);

        void Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const;
        void DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const;
};
