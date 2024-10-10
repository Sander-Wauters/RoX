#include "RoX/Timer.h"

Timer::Timer() :
    m_elapsedTicks(0),
    m_totalTicks(0),
    m_leftOverTicks(0),
    m_frameCount(0),
    m_framesPerSecond(0),
    m_framesThisSecond(0),
    m_qpcSecondCounter(0),
    m_isFixedTimeStep(false),
    m_targetElapsedTicks(TicksPerSecond / 60) {
    if (!QueryPerformanceFrequency(&m_qpcFrequency))
        throw std::exception();

    if (!QueryPerformanceCounter(&m_qpcLastTime))
        throw std::exception();

    // Initialize max delta to 1/10 of a second.
    m_qpcMaxDelta = static_cast<uint64_t>(m_qpcFrequency.QuadPart / 10);
}

void Timer::ResetElapsedTime() {
    if (!QueryPerformanceCounter(&m_qpcLastTime))
        throw std::exception();

    m_leftOverTicks = 0;
    m_framesPerSecond = 0;
    m_framesThisSecond = 0;
    m_qpcSecondCounter = 0;
}

void Timer::Tick() {
    Tick([](){});
}

std::uint64_t Timer::GetElapsedTicks() const noexcept { 
    return m_elapsedTicks; 
}

double Timer::GetElapsedSeconds() const noexcept { 
    return TicksToSeconds(m_elapsedTicks); 
}

std::uint64_t Timer::GetTotalTicks() const noexcept { 
    return m_totalTicks; 
}

double Timer::GetTotalSeconds() const noexcept { 
    return TicksToSeconds(m_totalTicks); 
}

std::uint32_t Timer::GetFrameCount() const noexcept { 
    return m_frameCount; 
}

std::uint32_t Timer::GetFramesPerSecond() const noexcept { 
    return m_framesPerSecond; 
}

void Timer::SetFixedTimeStep(bool isFixedTimestep) noexcept { 
    m_isFixedTimeStep = isFixedTimestep; 
}

void Timer::SetTargetElapsedTicks(uint64_t targetElapsed) noexcept { 
    m_targetElapsedTicks = targetElapsed; 
}

void Timer::SetTargetElapsedSeconds(double targetElapsed) noexcept { 
    m_targetElapsedTicks = SecondsToTicks(targetElapsed); 
}
