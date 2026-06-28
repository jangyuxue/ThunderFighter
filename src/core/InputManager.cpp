#include "core/InputManager.h"

void InputManager::Update() {
    // ---- 保存上一帧键盘状态 ----
    for (int i = 0; i < 256; ++i) {
        m_prevKeyStates[i] = m_keyStates[i];
    }

    // ---- 键盘轮询 ----
    m_keyStates[VK_LEFT]   = (GetAsyncKeyState(VK_LEFT)   & 0x8000) ? 1 : 0;
    m_keyStates[VK_RIGHT]  = (GetAsyncKeyState(VK_RIGHT)  & 0x8000) ? 1 : 0;
    m_keyStates[VK_UP]     = (GetAsyncKeyState(VK_UP)     & 0x8000) ? 1 : 0;
    m_keyStates[VK_DOWN]   = (GetAsyncKeyState(VK_DOWN)   & 0x8000) ? 1 : 0;
    m_keyStates['W'] = (GetAsyncKeyState('W') & 0x8000) ? 1 : 0;
    m_keyStates['A'] = (GetAsyncKeyState('A') & 0x8000) ? 1 : 0;
    m_keyStates['S'] = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
    m_keyStates['D'] = (GetAsyncKeyState('D') & 0x8000) ? 1 : 0;
    m_keyStates['J']        = (GetAsyncKeyState('J')        & 0x8000) ? 1 : 0;
    m_keyStates['K']        = (GetAsyncKeyState('K')        & 0x8000) ? 1 : 0;
    m_keyStates[VK_SPACE]   = (GetAsyncKeyState(VK_SPACE)   & 0x8000) ? 1 : 0;
    m_keyStates[VK_RETURN]  = (GetAsyncKeyState(VK_RETURN)  & 0x8000) ? 1 : 0;
    m_keyStates[VK_ESCAPE]  = (GetAsyncKeyState(VK_ESCAPE)  & 0x8000) ? 1 : 0;

    // ---- 鼠标 ----
    m_prevMouseDown = m_mouseDown;

    POINT pt;
    GetCursorPos(&pt);
    if (m_hWnd) {
        ScreenToClient(m_hWnd, &pt);
    }
    m_mouseX = pt.x;
    m_mouseY = pt.y;

    m_mouseDown    = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    m_mousePressed = m_mouseDown && !m_prevMouseDown;
    m_mouseReleased = !m_mouseDown && m_prevMouseDown;
}

// ---- 键盘方法 ----
bool InputManager::IsKeyDown(int vkCode) const {
    return m_keyStates[vkCode] != 0;
}

bool InputManager::IsKeyPressed(int vkCode) const {
    return m_keyStates[vkCode] != 0 && m_prevKeyStates[vkCode] == 0;
}

float InputManager::GetHorizontal() const {
    float dx = 0.0f;
    if (IsKeyDown(VK_LEFT)  || IsKeyDown('A')) dx -= 1.0f;
    if (IsKeyDown(VK_RIGHT) || IsKeyDown('D')) dx += 1.0f;
    return dx;
}

float InputManager::GetVertical() const {
    float dy = 0.0f;
    if (IsKeyDown(VK_UP)   || IsKeyDown('W')) dy -= 1.0f;
    if (IsKeyDown(VK_DOWN) || IsKeyDown('S')) dy += 1.0f;
    return dy;
}

bool InputManager::IsShooting()    const { return IsKeyDown('J'); }
bool InputManager::IsBombPressed() const { return IsKeyPressed('K'); }
bool InputManager::IsPausePressed() const { return IsKeyPressed(VK_SPACE); }
bool InputManager::IsConfirmPressed() const { return IsKeyPressed(VK_RETURN); }
bool InputManager::IsBackPressed()    const { return IsKeyPressed(VK_ESCAPE); }
