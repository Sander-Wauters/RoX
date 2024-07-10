#include "SubmeshDeviceData.h"

#include <DirectXHelpers.h>

#include "RoX/VertexTypes.h"

#include "Exceptions/ThrowIfFailed.h"

SubmeshDeviceData::SubmeshDeviceData(ID3D12Device* pDevice, Submesh* pSubmesh)
{}

void SubmeshDeviceData::Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), 1, pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
}

void SubmeshDeviceData::DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), pSubmesh->GetNumVisibleInstances(), pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
}

