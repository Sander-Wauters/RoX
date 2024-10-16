#include "DebugUI/DebugUI.h"

#include "DebugUI/SceneUI.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/TimerUI.h"

#include "ImGui/imgui.h"

DebugUI::DebugUI(Timer& mainTimer, Renderer& renderer, std::shared_ptr<Scene> pScene, std::uint8_t debugAssetBatchIndex) 
    : m_mainTimer(mainTimer),
    m_renderer(renderer),
    m_debugAssetBatchIndex(debugAssetBatchIndex),
    m_currentSceneGUID(pScene->GetGUID())
{
    Add(pScene);  
}

void DebugUI::Show() {
    const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    float width = 400.f;

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkSize.x - width, 0.f));
    ImGui::SetNextWindowSize(ImVec2(width, mainViewport->WorkSize.y));
    SceneUI::Window(*m_scenes.at(m_currentSceneGUID), windowFlags);

    std::uint64_t previousSceneGUID = m_currentSceneGUID;
    ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkSize.x - width - 200.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(200.f, 100.f));
    SceneUI::SelectorWindow(m_currentSceneGUID, m_scenes, windowFlags);
    if (previousSceneGUID != m_currentSceneGUID)
        UpdateScheduler::Get().Add([&]() { m_renderer.Load(*m_scenes.at(m_currentSceneGUID)); });

    
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(350.f, 250.f));
    TimerUI::Window(m_mainTimer, windowFlags);
}

void DebugUI::Add(std::shared_ptr<Scene> pScene) {
    AddWorldGrid(*pScene->GetAssetBatch(m_debugAssetBatchIndex));
    AddWorldAxis(*pScene->GetAssetBatch(m_debugAssetBatchIndex));
    m_scenes[pScene->GetGUID()] = pScene;
}

void DebugUI::RemoveScene(std::uint64_t GUID) {
    m_scenes.erase(GUID);
}

void DebugUI::AddWorldGrid(AssetBatch& batch) {
    GridOutline grid(
            "world_grid", 10000, 10000, 
            { 10000.f, 0.f, 0.f }, 
            { 0.f, 0.f, 10000.f }, 
            { 0.f, -0.005f, 0.f }, 
            DirectX::Colors::Gray);

    if (batch.FindGUID(grid.GetName(), batch.GetOutlines()) == Identifiable::INVALID_GUID)
        batch.Add(std::make_shared<GridOutline>(grid));
}

void DebugUI::AddWorldAxis(AssetBatch& batch) {
    RayOutline xAxis("world_x_axis", { 10000.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, DirectX::Colors::Red);
    RayOutline yAxis("world_y_axis", { 0.f, 10000.f, 0.f }, { 0.f, 0.f, 0.f }, DirectX::Colors::LimeGreen);
    RayOutline zAxis("world_z_axis", { 0.f, 0.f, 10000.f }, { 0.f, 0.f, 0.f }, DirectX::Colors::Blue);

    if (batch.FindGUID(xAxis.GetName(), batch.GetOutlines()) == Identifiable::INVALID_GUID)
        batch.Add(std::make_shared<RayOutline>(xAxis));
    if (batch.FindGUID(yAxis.GetName(), batch.GetOutlines()) == Identifiable::INVALID_GUID)
        batch.Add(std::make_shared<RayOutline>(yAxis));
    if (batch.FindGUID(zAxis.GetName(), batch.GetOutlines()) == Identifiable::INVALID_GUID)
        batch.Add(std::make_shared<RayOutline>(zAxis));
}

std::shared_ptr<Scene> DebugUI::GetCurrentScene() const noexcept {
    return m_scenes.at(m_currentSceneGUID);
}

