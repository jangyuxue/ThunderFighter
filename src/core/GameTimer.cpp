#include "core/GameTimer.h"

GameTimer::GameTimer() {
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_startTime);
    m_lastTime = m_startTime;
}

double GameTimer::Tick() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    double elapsed = static_cast<double>(now.QuadPart - m_lastTime.QuadPart)
                     / m_frequency.QuadPart;
    m_lastTime = now;

    // 限制最大帧时间，防止调试断点后暴走
    if (elapsed > 0.25) elapsed = 0.25;

    // FPS 计算
    ++m_frameCount;
    m_fpsAccumulator += elapsed;
    if (m_fpsAccumulator >= 1.0) {
        m_fps = static_cast<float>(m_frameCount) / static_cast<float>(m_fpsAccumulator);
        m_frameCount = 0;
        m_fpsAccumulator = 0.0;
    }

    return elapsed;
}

void GameTimer::Reset() {
    QueryPerformanceCounter(&m_startTime);
    m_lastTime = m_startTime;
    m_frameCount = 0;
    m_fps = 0.0f;
    m_fpsAccumulator = 0.0;
}

double GameTimer::GetTotalTime() const {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart - m_startTime.QuadPart)
           / m_frequency.QuadPart;
}
