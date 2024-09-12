#include "DeviceResourceData.h"

DeviceResourceData::DeviceResourceData(DeviceResources& deviceResources) 
    noexcept : m_pScene(nullptr),
    m_deviceResources(deviceResources)
{
    m_deviceResources.RegisterDeviceObserver(this);
}

DeviceResourceData::~DeviceResourceData() noexcept {
}

void DeviceResourceData::OnDeviceLost() {
    m_pImGuiDescriptorHeap.reset();
    m_pImGuiStates.reset();
}

void DeviceResourceData::OnDeviceRestored() {

}

void DeviceResourceData::Load(Scene& scene, bool& msaaEnabled) {
    if (!m_pScene)
        FreshLoad(scene, msaaEnabled);
    else
        DirtyLoad(scene, msaaEnabled);
}

void DeviceResourceData::UpdateEffects() {
    // TODO: Client should be able to decide this.
    // Store the view and projection in the object that the client has access to.
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_pScene->GetCamera().GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_pScene->GetCamera().GetProjection());

    for (std::uint8_t i = 0; i < GetNumDataBatches(); ++i) {
        m_dataBatches[i]->UpdateEffects(view, projection);
    }
}

void DeviceResourceData::CreateDeviceDependentResources() {
    CreateImGuiResources();
}

void DeviceResourceData::CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch) {
    for (std::unique_ptr<DeviceDataBatch>& pBatch : m_dataBatches) {
        pBatch->CreateRenderTargetDependentResources(resourceUploadBatch);
    }
}

void DeviceResourceData::CreateWindowSizeDependentResources() {
    for (std::unique_ptr<DeviceDataBatch>& pBatch : m_dataBatches) {
        pBatch->CreateWindowSizeDependentResources();
    }
}

void DeviceResourceData::CreateImGuiResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    m_pImGuiDescriptorHeap = std::make_unique<DirectX::DescriptorHeap>(
            pDevice,
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            1);
    m_pImGuiStates = std::make_unique<DirectX::CommonStates>(pDevice);
}

void DeviceResourceData::FreshLoad(Scene& scene, bool& msaaEnabled) {
    m_pScene = &scene;
    m_dataBatches.reserve(m_pScene->GetNumAssetBatches());
    for (std::uint8_t i = 0; i < m_pScene->GetNumAssetBatches(); ++i) {
        auto pAssetBatch = m_pScene->GetAssetBatches()[i];

        auto pDataBatch = std::make_unique<DeviceDataBatch>(m_deviceResources, pAssetBatch->GetMaxNumTextures(), msaaEnabled);
        pDataBatch->Add(*m_pScene->GetAssetBatches()[i]);

        pAssetBatch->RegisterAssetBatchObserver(pDataBatch.get());
        m_dataBatches.push_back(std::move(pDataBatch));
    }
}

void DeviceResourceData::DirtyLoad(Scene& scene, bool& msaaEnabled) {
    // Deregister the data batches.
    for (std::uint8_t i = 0; i < m_pScene->GetNumAssetBatches(); ++i) {
        m_pScene->GetAssetBatches()[i]->DeregisterAssetBatchObserver(m_dataBatches[i].get());
        m_deviceResources.DeregisterDeviceObserver(m_dataBatches[i].get());
    }

    // Find all batches that are already loaded.
    std::vector<std::string> currentlyLoadedBatches;
    std::vector<std::unique_ptr<DeviceDataBatch>> currentlyLoadedDataBatches;
    for (std::uint8_t i = 0; i < m_pScene->GetNumAssetBatches(); ++i) {
        for (std::uint8_t j = 0; j < scene.GetNumAssetBatches(); ++j) {
            if (m_pScene->GetAssetBatches()[i]->GetName() == scene.GetAssetBatches()[j]->GetName()) {
                currentlyLoadedBatches.push_back(scene.GetAssetBatches()[i]->GetName());
                currentlyLoadedDataBatches.push_back(std::move(m_dataBatches[i]));
                break;
            }
        }
    }

    // Load in the new batches and move over the previously loaded batches.
    m_dataBatches.clear();
    for (std::uint8_t i = 0; i < scene.GetNumAssetBatches(); ++i) {
        auto pAssetBatch = scene.GetAssetBatches()[i];

        auto pos = std::find(currentlyLoadedBatches.begin(), currentlyLoadedBatches.end(), scene.GetAssetBatches()[i]->GetName()) - currentlyLoadedBatches.begin();
        if (pos < currentlyLoadedBatches.size()) {
            pAssetBatch->RegisterAssetBatchObserver(currentlyLoadedDataBatches[pos].get());
            m_dataBatches.push_back(std::move(currentlyLoadedDataBatches[pos]));
        } else {
            auto pDataBatch = std::make_unique<DeviceDataBatch>(m_deviceResources, scene.GetAssetBatches()[i]->GetMaxNumTextures(), msaaEnabled);
            pDataBatch->Add(*scene.GetAssetBatches()[i]);

            pAssetBatch->RegisterAssetBatchObserver(pDataBatch.get());
            m_dataBatches.push_back(std::move(pDataBatch));
        }
    }
    m_pScene = &scene;
}

Scene& DeviceResourceData::GetScene() const noexcept {
    return *m_pScene;
}

std::uint8_t DeviceResourceData::GetNumDataBatches() const noexcept {
    return m_pScene->GetNumAssetBatches();
}

const std::vector<std::unique_ptr<DeviceDataBatch>>& DeviceResourceData::GetDataBatches() const noexcept {
    return m_dataBatches;
}

DirectX::DescriptorHeap* DeviceResourceData::GetImGuiDescriptorHeap() noexcept {
    return m_pImGuiDescriptorHeap.get();
}

DirectX::CommonStates* DeviceResourceData::GetImGuiStates() noexcept {
    return m_pImGuiStates.get();
}

bool DeviceResourceData::SceneLoaded() const noexcept {
    return m_pScene != nullptr;
}

