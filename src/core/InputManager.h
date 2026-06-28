#pragma once

#include <windows.h>

// ============================================================
// 键盘 + 鼠标输入管理
// ============================================================
class InputManager {
public:
    InputManager() = default;

    void SetWindow(HWND hWnd) { m_hWnd = hWnd; }

    // 每帧调用一次
    void Update();

    // ---- 键盘 ----
    bool IsKeyDown(int vkCode) const;
    bool IsKeyPressed(int vkCode) const;

    float GetHorizontal() const;
    float GetVertical()   const;
    bool  IsShooting()    const;
    bool  IsBombPressed() const;
    bool  IsPausePressed() const;
    bool  IsConfirmPressed() const;
    bool  IsBackPressed()    const;

    // ---- 鼠标 ----
    int  GetMouseX() const { return m_mouseX; }
    int  GetMouseY() const { return m_mouseY; }
    bool IsMouseDown()    const { return m_mouseDown; }
    bool IsMousePressed() const { return m_mousePressed; }
    bool IsMouseReleased() const { return m_mouseReleased; }

private:
    HWND  m_hWnd = nullptr;

    // 键盘
    BYTE  m_keyStates[256]     = {};
    BYTE  m_prevKeyStates[256] = {};

    // 鼠标
    int   m_mouseX = 0, m_mouseY = 0;
    bool  m_mouseDown     = false;
    bool  m_prevMouseDown = false;
    bool  m_mousePressed  = false;
    bool  m_mouseReleased = false;
};
