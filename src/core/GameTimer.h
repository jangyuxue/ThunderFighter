#pragma once

#include <windows.h>

// ============================================================
// 高精度计时器 — 基于 QueryPerformanceCounter
// ============================================================
class GameTimer {
public:
    GameTimer();

    // 返回自上一次 Tick 以来的时间（秒），首次调用返回 0
    double Tick();

    // 重置计时器
    void Reset();

    // 获取当前 FPS（每秒更新一次）
    float GetFPS() const { return m_fps; }

    // 获取自游戏开始的总时间
    double GetTotalTime() const;

private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_lastTime;
    int           m_frameCount    = 0;
    float         m_fps           = 0.0f;
    double        m_fpsAccumulator = 0.0;
};
