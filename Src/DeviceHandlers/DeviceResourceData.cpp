#include "DeviceResourceData.h"

#include <ImGuiBackends/imgui_impl_dx12.h>

DeviceResourceData::DeviceResourceData(DeviceResources& deviceResources, bool msaaEnabled) 
    noexcept : m_pScene(nullptr),
    m_deviceResources(deviceResources),
    m_msaaEnabled(msaaEnabled)
{
    m_deviceResources.Attach(this);
    CreateDeviceDependentResources();
    ImGui_ImplDX12_Init(m_deviceResources.GetDevice(), 
            m_deviceResources.GetBackBufferCount(),
            m_deviceResources.GetBackBufferFormat(),
            m_pImGuiDescriptorHeap->Heap(),
            m_pImGuiDescriptorHeap->GetCpuHandle(0),
            m_pImGuiDescriptorHeap->GetGpuHandle(0));
}

DeviceResourceData::~DeviceResourceData() noexcept {
    m_deviceResources.Detach(this);
}

void DeviceResourceData::OnDeviceLost() {
    m_pImGuiDescriptorHeap.reset();
    m_pCommonStates.reset();
    ImGui_ImplDX12_Shutdown();
}

void DeviceResourceData::OnDeviceRestored() {
    CreateDeviceDependentResources();
    ImGui_ImplDX12_Init(m_deviceResources.GetDevice(), 
            m_deviceResources.GetBackBufferCount(),
            m_deviceResources.GetBackBufferFormat(),
            m_pImGuiDescriptorHeap->Heap(),
            m_pImGuiDescriptorHeap->GetCpuHandle(0),
            m_pImGuiDescriptorHeap->GetGpuHandle(0));
    for (auto& dataBatch : m_dataBatches) {
        dataBatch->SetCommonStates(m_pCommonStates.get());
        dataBatch->SetRtState(m_pRenderTartgetState.get());
    }
}

void DeviceResourceData::Load(Scene& scene, bool& msaaEnabled) {
    if (!m_pScene)
        FreshLoad(scene, msaaEnabled);
    else
        DirtyLoad(scene, msaaEnabled);
}

void DeviceResourceData::Update() {
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&m_pScene->GetCamera().GetView());
    DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&m_pScene->GetCamera().GetProjection());

    for (std::uint8_t i = 0; i < GetNumDataBatches(); ++i) {
        m_dataBatches[i]->Update(view, projection);
    }
}

void DeviceResourceData::CreateDeviceDependentResources() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    m_pRenderTartgetState = std::make_unique<DirectX::RenderTargetState>(
            m_deviceResources.GetBackBufferFormat(), 
            m_deviceResources.GetDepthBufferFormat());
    if (m_msaaEnabled) {
        m_pRenderTartgetState->sampleDesc.Count = DeviceResources::MSAA_COUNT;
        m_pRenderTartgetState->sampleDesc.Quality = DeviceResources::MSAA_QUALITY;
    }
    m_pCommonStates = std::make_unique<DirectX::CommonStates>(pDevice);
    CreateImGuiResources();
}

void DeviceResourceData::CreateRenderTargetDependentResources(bool msaaEnabled) {
    m_msaaEnabled = msaaEnabled;
    if (msaaEnabled) {
        m_pRenderTartgetState->sampleDesc.Count = DeviceResources::MSAA_COUNT;
        m_pRenderTartgetState->sampleDesc.Quality = DeviceResources::MSAA_QUALITY;
    } else {
        m_pRenderTartgetState->sampleDesc.Count = 1;
        m_pRenderTartgetState->sampleDesc.Quality = 0;
    }

    for (std::unique_ptr<DeviceDataBatch>& pBatch : m_dataBatches) {
        pBatch->CreateRenderTargetDependentResources();
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
}

void DeviceResourceData::FreshLoad(Scene& scene, bool& msaaEnabled) {
    m_pScene = &scene;
    m_dataBatches.reserve(m_pScene->GetNumAssetBatches());
    for (std::uint8_t i = 0; i < m_pScene->GetNumAssetBatches(); ++i) {
        auto pAssetBatch = m_pScene->GetAssetBatches()[i];

        auto pDataBatch = std::make_unique<DeviceDataBatch>(
                m_deviceResources, 
                m_pCommonStates.get(),
                m_pRenderTartgetState.get(),
                pAssetBatch->GetMaxNumUniqueTextures());

        pDataBatch->Add(*m_pScene->GetAssetBatches()[i]);

        pAssetBatch->Attach(pDataBatch.get());
        m_dataBatches.push_back(std::move(pDataBatch));
    }
}

void DeviceResourceData::DirtyLoad(Scene& scene, bool& msaaEnabled) {
    // Detach the data batches.
    for (std::uint8_t i = 0; i < m_pScene->GetNumAssetBatches(); ++i) {
        m_pScene->GetAssetBatches()[i]->Detach(m_dataBatches[i].get());
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
            pAssetBatch->Attach(currentlyLoadedDataBatches[pos].get());
            m_dataBatches.push_back(std::move(currentlyLoadedDataBatches[pos]));
        } else {
            auto pDataBatch = std::make_unique<DeviceDataBatch>(
                    m_deviceResources, 
                    m_pCommonStates.get(),
                    m_pRenderTartgetState.get(),
                    pAssetBatch->GetMaxNumUniqueTextures());
            pDataBatch->Add(*scene.GetAssetBatches()[i]);

            pAssetBatch->Attach(pDataBatch.get());
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

DirectX::CommonStates* DeviceResourceData::GetCommonStates() noexcept {
    return m_pCommonStates.get();
}

bool DeviceResourceData::SceneLoaded() const noexcept {
    return m_pScene != nullptr;
}

