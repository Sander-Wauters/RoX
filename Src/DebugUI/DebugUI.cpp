#include "DebugUI/DebugUI.h"

#include "DebugUI/SceneUI.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/TimerUI.h"
#include "DebugUI/AnimationUI.h"

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

    static bool currentSceneWindowOpen = true;
    ImVec2 currentSceneWindowSize(400.f, mainViewport->WorkSize.y);
    ImVec2 currentSceneWindowPos(mainViewport->WorkSize.x - currentSceneWindowSize.x, 0.f);

    static bool sceneSelectorWindowOpen = true;
    ImVec2 sceneSelectorWindowSize(200.f, 100.f);
    ImVec2 sceneSelectorWindowPos(currentSceneWindowPos.x - sceneSelectorWindowSize.x, 0.f);

    static bool timerWindowOpen = true;
    ImVec2 timerWindowSize(350.f, 250.f);
    ImVec2 timerWindowPos(0.f, 0.f);

    static bool animationInfoWindowOpen = true;
    ImVec2 animationInfoWindowSize(350.f, mainViewport->WorkSize.y - timerWindowSize.y);
    ImVec2 animationInfoWindowPos(0.f, timerWindowSize.y);

    ImGuiWindowFlags windowFlags = 0
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoFocusOnAppearing;

    IMGUI_CHECKVERSION();

    ImGui::SetNextWindowSize(currentSceneWindowSize);
    ImGui::SetNextWindowPos(currentSceneWindowPos);
    if (ImGui::Begin("Current scene", &currentSceneWindowOpen, windowFlags))
        SceneUI::Menu(*m_scenes.at(m_currentSceneGUID));
    ImGui::End();

    std::uint64_t previousSceneGUID = m_currentSceneGUID;
    ImGui::SetNextWindowSize(sceneSelectorWindowSize);
    ImGui::SetNextWindowPos(sceneSelectorWindowPos);
    if (ImGui::Begin("Scene selector", &sceneSelectorWindowOpen, windowFlags))
        SceneUI::Selector(m_currentSceneGUID, m_scenes);
    ImGui::End();
    if (previousSceneGUID != m_currentSceneGUID)
        UpdateScheduler::Get().Add([&]() { m_renderer.Load(*m_scenes.at(m_currentSceneGUID)); });

    ImGui::SetNextWindowSize(timerWindowSize);
    ImGui::SetNextWindowPos(timerWindowPos);
    if (ImGui::Begin("Frame stats", &timerWindowOpen, windowFlags))
        TimerUI::Menu(m_mainTimer);
    ImGui::End();

    ImGui::SetNextWindowSize(animationInfoWindowSize);
    ImGui::SetNextWindowPos(animationInfoWindowPos);
    if (ImGui::Begin("Animations", &animationInfoWindowOpen, windowFlags))
        AnimationUI::Menu(m_animationInfo, *m_scenes.at(m_currentSceneGUID));
    ImGui::End();
    
}

void DebugUI::Add(std::shared_ptr<Scene> pScene) {
    AddWorldGrid(*pScene->GetAssetBatch(m_debugAssetBatchIndex));
    AddWorldAxis(*pScene->GetAssetBatch(m_debugAssetBatchIndex));
    m_scenes[pScene->GetGUID()] = pScene;
}

void DebugUI::Add(Animation& animation) {
    AnimationInfo animInfo(animation);
    m_animationInfo[animation.GetGUID()] = std::make_shared<AnimationInfo>(animInfo);
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

