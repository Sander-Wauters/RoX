#pragma once

#include <queue>
#include <functional>

class UpdateScheduler {
    private:
        UpdateScheduler();

    public:
        UpdateScheduler(UpdateScheduler&) = delete;
        void operator= (UpdateScheduler&) = delete;

        static UpdateScheduler& Get();

        void Add(std::function<void()> update);
        void Update();

    private:
        std::queue<std::function<void()>> m_updateQueue;
};
