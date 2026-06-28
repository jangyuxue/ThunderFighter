#include "ui/Button.h"
#include <cmath>

void Button::Init(float x, float y, float w, float h,
                  const std::wstring& text,
                  Gdiplus::Color bgColor,
                  Gdiplus::Color hoverColor,
                  Gdiplus::Color textColor) {
    m_x = x; m_y = y; m_w = w; m_h = h;
    m_text = text;
    m_bgColor = bgColor;
    m_hoverColor = hoverColor;
    m_textColor = textColor;
}

bool Button::IsHovered(int mx, int my) const {
    float hw = m_w * 0.5f, hh = m_h * 0.5f;
    return mx >= m_x - hw && mx <= m_x + hw
        && my >= m_y - hh && my <= m_y + hh;
}

void Button::Update(int mx, int my, bool mouseDown, bool mousePressed) {
    m_hovered = IsHovered(mx, my) && m_enabled;
    m_pressed = m_hovered && mouseDown;
    m_clicked = m_hovered && mousePressed;
    if (m_pressed) m_animScale += (0.93f - m_animScale) * 0.35f;
    else           m_animScale += (1.0f - m_animScale) * 0.25f;
}

void Button::Render(Gdiplus::Graphics& g, Gdiplus::Font& font) {
    float hw = m_w * m_animScale * 0.5f, hh = m_h * m_animScale * 0.5f;
    float bx = m_x - hw, by = m_y - hh;
    float bw = m_w * m_animScale, bh = m_h * m_animScale;
    float r = 10.0f;  // 圆角半径

    // 发光层（悬停时）
    if (m_hovered && !m_locked) {
        Gdiplus::Color glow(80, 255, 230, 120);
        Gdiplus::SolidBrush glowBrush(glow);
        g.FillRectangle(&glowBrush, bx - 3.0f, by - 3.0f, bw + 6.0f, bh + 6.0f);
        g.FillRectangle(&glowBrush, bx - 1.0f, by - 5.0f, bw + 2.0f, bh + 10.0f);
        g.FillRectangle(&glowBrush, bx - 5.0f, by - 1.0f, bw + 10.0f, bh + 2.0f);
    }

    // 背景圆角矩形
    Gdiplus::Color bg = m_bgColor;
    if (m_locked)           bg = Gdiplus::Color(160, 20, 20, 25);
    else if (m_owned)       bg = Gdiplus::Color(190, 15, 50, 25);
    else if (m_pressed)     bg = Gdiplus::Color(240, 30, 50, 80);
    else if (m_hovered)     bg = m_hoverColor;

    Gdiplus::SolidBrush bgBrush(bg);
    // 用 GraphicsPath 画真正的圆角矩形
    Gdiplus::GraphicsPath path;
    path.AddArc(bx, by, r * 2, r * 2, 180, 90);
    path.AddArc(bx + bw - r * 2, by, r * 2, r * 2, 270, 90);
    path.AddArc(bx + bw - r * 2, by + bh - r * 2, r * 2, r * 2, 0, 90);
    path.AddArc(bx, by + bh - r * 2, r * 2, r * 2, 90, 90);
    path.CloseFigure();
    g.FillPath(&bgBrush, &path);

    // 边框
    if (!m_locked) {
        Gdiplus::Color borderColor = m_borderColor;
        if (m_hovered) borderColor = Gdiplus::Color(255, 255, 240, 160);
        Gdiplus::Pen border(borderColor, m_hovered ? 2.0f : 1.2f);
        g.DrawPath(&border, &path);
    }

    // 文字
    if (!m_text.empty()) {
        Gdiplus::Color tc = m_locked ? Gdiplus::Color(80, 80, 80, 100)
                           : m_owned ? Gdiplus::Color(140, 80, 220, 80)
                           : m_textColor;
        Gdiplus::SolidBrush textBrush(tc);
        Gdiplus::StringFormat fmt;
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::RectF textRect(bx, by, bw, bh);
        g.DrawString(m_text.c_str(), -1, &font, textRect, &fmt, &textBrush);
    }
}
