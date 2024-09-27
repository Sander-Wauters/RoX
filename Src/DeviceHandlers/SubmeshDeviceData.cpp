#include "SubmeshDeviceData.h"

SubmeshDeviceData::SubmeshDeviceData(ID3D12Device* pDevice, Submesh* pSubmesh)
{}

void SubmeshDeviceData::Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    pCommandList->DrawIndexedInstanced(
            pSubmesh->GetIndexCount(), 
            1, 
            pSubmesh->GetStartIndex(), 
            pSubmesh->GetVertexOffset(), 
            pSubmesh->GetNumCulled());
}

void SubmeshDeviceData::DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    pCommandList->DrawIndexedInstanced(
            pSubmesh->GetIndexCount(), 
            pSubmesh->GetNumInstances(), 
            pSubmesh->GetStartIndex(), 
            pSubmesh->GetVertexOffset(), 
            pSubmesh->GetNumCulled());
}

