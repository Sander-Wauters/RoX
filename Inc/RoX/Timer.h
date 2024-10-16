#pragma once

#include <cmath>
#include <cstdint>
#include <exception>
#include <functional>
#include <Windows.h>

// Helper class to deal with time based events.
class Timer {
    public:
        Timer();

    public:
        static constexpr double TicksToSeconds(uint64_t ticks) noexcept { return static_cast<double>(ticks) / TicksPerSecond; }
        static constexpr std::uint64_t SecondsToTicks(double seconds) noexcept { return static_cast<uint64_t>(seconds * TicksPerSecond); }

        // After an intentional timing discontinuity (for instance a blocking IO operation)
        // call this to avoid having the fixed timestep logic attempt a set of catch-up
        // Update calls.
        void ResetElapsedTime();


        // Update timer state, calling the specified Update function the appropriate number of times.
        void Tick();
        void Tick(const std::function<void()>& update);

    public:
        // Get elapsed time since the previous Update call.
        std::uint64_t GetElapsedTicks() const noexcept;
        double GetElapsedSeconds() const noexcept;

        // Get total time since the start of the program.
        std::uint64_t GetTotalTicks() const noexcept;
        double GetTotalSeconds() const noexcept;

        // Get total number of updates since start of the program.
        std::uint32_t GetFrameCount() const noexcept;

        // Get the current framerate.
        std::uint32_t GetFramesPerSecond() const noexcept;

        // Set whether to use fixed or variable timestep mode.
        void SetFixedTimeStep(bool isFixedTimestep) noexcept;

        bool IsUsingFixedTimeStep() const noexcept;

        // Set how often to call Update when in fixed timestep mode.
        void SetTargetElapsedTicks(std::uint64_t targetElapsed) noexcept;
        void SetTargetElapsedSeconds(double targetElapsed) noexcept;

        std::uint64_t GetTargetElapsedTicks() const noexcept;
        double GetTargetElapsedSeconds() const noexcept;

    private:
        // Integer format represents time using 10,000,000 ticks per second.
        static constexpr std::uint64_t TicksPerSecond = 10000000;

        // Source timing data uses QPC units.
        LARGE_INTEGER m_qpcFrequency;
        LARGE_INTEGER m_qpcLastTime;
        std::uint64_t m_qpcMaxDelta;

        // Derived timing data uses a canonical tick format.
        std::uint64_t m_elapsedTicks;
        std::uint64_t m_totalTicks;
        std::uint64_t m_leftOverTicks;

        // Members for tracking the framerate.
        std::uint32_t m_frameCount;
        std::uint32_t m_framesPerSecond;
        std::uint32_t m_framesThisSecond;
        std::uint64_t m_qpcSecondCounter;

        // Members for configuring fixed timestep mode.
        bool m_isFixedTimeStep;
        std::uint64_t m_targetElapsedTicks;
};
