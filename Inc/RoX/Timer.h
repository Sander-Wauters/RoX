#pragma once

#include <cmath>
#include <cstdint>
#include <exception>

#include "../../Src/Util/pch.h"

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
        template<typename TUpdate>
            void Tick(const TUpdate& update) {
                // Query the current time.
                LARGE_INTEGER currentTime;

                if (!QueryPerformanceCounter(&currentTime))
                    throw std::exception();

                std::uint64_t timeDelta = static_cast<std::uint64_t>(currentTime.QuadPart - m_qpcLastTime.QuadPart);

                m_qpcLastTime = currentTime;
                m_qpcSecondCounter += timeDelta;

                // Clamp excessively large time deltas (e.g. after paused in the debugger).
                if (timeDelta > m_qpcMaxDelta)
                    timeDelta = m_qpcMaxDelta;

                // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
                timeDelta *= TicksPerSecond;
                timeDelta /= static_cast<std::uint64_t>(m_qpcFrequency.QuadPart);

                const std::uint32_t lastFrameCount = m_frameCount;

                if (m_isFixedTimeStep) {
                    // Fixed timestep update logic

                    // If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
                    // the clock to exactly match the target value. This prevents tiny and irrelevant errors
                    // from accumulating over time. Without this clamping, a game that requested a 60 fps
                    // fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
                    // accumulate enough tiny errors that it would drop a frame. It is better to just round
                    // small deviations down to zero to leave things running smoothly.

                    if (static_cast<std::uint64_t>(std::abs(static_cast<std::int64_t>(timeDelta - m_targetElapsedTicks))) < TicksPerSecond / 4000)
                        timeDelta = m_targetElapsedTicks;

                    m_leftOverTicks += timeDelta;

                    while (m_leftOverTicks >= m_targetElapsedTicks) {
                        m_elapsedTicks = m_targetElapsedTicks;
                        m_totalTicks += m_targetElapsedTicks;
                        m_leftOverTicks -= m_targetElapsedTicks;
                        m_frameCount++;

                        update();
                    }
                } else {
                    // Variable timestep update logic.
                    m_elapsedTicks = timeDelta;
                    m_totalTicks += timeDelta;
                    m_leftOverTicks = 0;
                    m_frameCount++;

                    update();
                }

                // Track the current framerate.
                if (m_frameCount != lastFrameCount)
                    m_framesThisSecond++;

                if (m_qpcSecondCounter >= static_cast<std::uint64_t>(m_qpcFrequency.QuadPart)) {
                    m_framesPerSecond = m_framesThisSecond;
                    m_framesThisSecond = 0;
                    m_qpcSecondCounter %= static_cast<std::uint64_t>(m_qpcFrequency.QuadPart);
                }
            }

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

        // Set how often to call Update when in fixed timestep mode.
        void SetTargetElapsedTicks(uint64_t targetElapsed) noexcept;
        void SetTargetElapsedSeconds(double targetElapsed) noexcept;

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
