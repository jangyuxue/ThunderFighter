#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include <string>
#include <functional>

// ============================================================
// 可点击按钮组件
// ============================================================
class Button {
public:
    Button() = default;

    void Init(float x, float y, float w, float h,
              const std::wstring& text,
              Gdiplus::Color bgColor,
              Gdiplus::Color hoverColor,
              Gdiplus::Color textColor = Gdiplus::Color(255, 255, 255, 255));

    // 鼠标交互
    bool IsHovered(int mx, int my) const;
    void Update(int mx, int my, bool mouseDown, bool mousePressed);
    bool IsClicked() const { return m_clicked; }

    // 渲染
    void Render(Gdiplus::Graphics& g, Gdiplus::Font& font);

    // 属性
    void SetPosition(float x, float y) { m_x = x; m_y = y; }
    void SetText(const std::wstring& text) { m_text = text; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

    // 状态
    void SetOwned(bool owned) { m_owned = owned; }
    bool IsOwned() const { return m_owned; }
    void SetLocked(bool locked) { m_locked = locked; }
    bool IsLocked() const { return m_locked; }

private:
    float m_x = 0, m_y = 0;
    float m_w = 160, m_h = 50;
    std::wstring m_text;

    Gdiplus::Color m_bgColor     = Gdiplus::Color(200, 30, 30, 50);
    Gdiplus::Color m_hoverColor  = Gdiplus::Color(200, 50, 50, 80);
    Gdiplus::Color m_textColor   = Gdiplus::Color(255, 255, 255, 255);
    Gdiplus::Color m_borderColor = Gdiplus::Color(200, 255, 220, 80);

    bool m_hovered  = false;
    bool m_pressed  = false;
    bool m_clicked  = false;
    bool m_enabled  = true;
    bool m_owned    = false;    // 已拥有（商店用）
    bool m_locked   = false;    // 已锁定（关卡用）

    float m_animScale = 1.0f;   // 按下缩放动画
};
