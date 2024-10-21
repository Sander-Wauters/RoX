#include "DebugUI/TimerUI.h"

#include "ImGui/imgui.h"

void TimerUI::TimeStepControls(Timer& timer) {
    static bool fixedTimestep = timer.IsUsingFixedTimeStep();
    if (ImGui::Checkbox("Fixed time step", &fixedTimestep))
        timer.SetFixedTimeStep(fixedTimestep);

    static float targetElapsed = 60.f;
    if (ImGui::DragFloat("Target fps", &targetElapsed))
        timer.SetTargetElapsedSeconds(1 / targetElapsed);
}

void TimerUI::Stats(Timer& timer) {
    ImGui::Text("Ticks elapsed:   %llu", timer.GetElapsedTicks());
    ImGui::Text("Seconds elapsed: %f", timer.GetElapsedSeconds());
    ImGui::Separator();
    ImGui::Text("Total ticks:   %llu", timer.GetTotalTicks());
    ImGui::Text("Total Seconds: %f", timer.GetTotalSeconds());
    ImGui::Separator();
    ImGui::Text("Frames elapsed:    %d", timer.GetFrameCount());
    ImGui::Text("Frames per second: %d", timer.GetFramesPerSecond());
}

void TimerUI::Menu(Timer& timer) {
    ImGui::SeparatorText("Time step controls");
    TimeStepControls(timer);
    ImGui::SeparatorText("Statistics");
    Stats(timer);
}

