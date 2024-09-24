#include "DebugUI/UpdateScheduler.h"

UpdateScheduler::UpdateScheduler() {
}

UpdateScheduler& UpdateScheduler::Get() {
    static UpdateScheduler instance;
    return instance;
}

void UpdateScheduler::Add(std::function<void()> update) {
    m_updateQueue.push(update);
}

void UpdateScheduler::Update() {
    while (!m_updateQueue.empty()) {
        m_updateQueue.front()();
        m_updateQueue.pop();
    }
}
