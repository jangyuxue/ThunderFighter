#pragma once

#include <windows.h>

// ============================================================
// 键盘输入管理 — 基于 GetAsyncKeyState 实时轮询
// ============================================================
class InputManager {
public:
    InputManager() = default;

    // 每帧调用一次，轮询所有关键按键状态
    void Update();

    // 按键是否被按住（连续检测，用于移动和射击）
    bool IsKeyDown(int vkCode) const;

    // 按键是否刚按下（边缘触发，用于菜单和一次性操作）
    bool IsKeyPressed(int vkCode) const;

    // 便捷移动输入
    float GetHorizontal() const;  // -1（左）到 +1（右）
    float GetVertical()   const;  // -1（上）到 +1（下）
    bool  IsShooting()    const;  // J 键
    bool  IsBombPressed() const;  // K 键（边缘触发）
    bool  IsPausePressed() const; // 空格（边缘触发）
    bool  IsConfirmPressed() const; // Enter（边缘触发）
    bool  IsBackPressed()    const; // ESC（边缘触发）

private:
    BYTE  m_keyStates[256]     = {};
    BYTE  m_prevKeyStates[256] = {};
};
