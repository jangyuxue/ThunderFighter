#include "ui/Button.h"

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
    float hw = m_w * 0.5f;
    float hh = m_h * 0.5f;
    return mx >= m_x - hw && mx <= m_x + hw
        && my >= m_y - hh && my <= m_y + hh;
}

void Button::Update(int mx, int my, bool mouseDown, bool mousePressed) {
    m_hovered = IsHovered(mx, my) && m_enabled;
    m_pressed = m_hovered && mouseDown;
    m_clicked = m_hovered && mousePressed;

    // 缩放动画
    if (m_pressed) {
        m_animScale += (0.92f - m_animScale) * 0.3f;
    } else {
        m_animScale += (1.0f - m_animScale) * 0.3f;
    }
}

void Button::Render(Gdiplus::Graphics& g, Gdiplus::Font& font) {
    float hw = m_w * m_animScale * 0.5f;
    float hh = m_h * m_animScale * 0.5f;
    float x = m_x - hw;
    float y = m_y - hh;
    float w = m_w * m_animScale;
    float h = m_h * m_animScale;

    // 背景
    Gdiplus::Color bg = m_bgColor;
    if (m_locked) {
        bg = Gdiplus::Color(150, 40, 40, 40);
    } else if (m_owned) {
        bg = Gdiplus::Color(180, 20, 80, 30);
    } else if (m_hovered && m_enabled) {
        bg = m_hoverColor;
    }

    Gdiplus::SolidBrush bgBrush(bg);
    Gdiplus::Pen borderPen(m_borderColor, m_hovered ? 2.5f : 1.5f);

    // 圆角矩形效果（用 FillRectangle + 小圆角近似）
    g.FillRectangle(&bgBrush, x + 4.0f, y, w - 8.0f, h);
    g.FillRectangle(&bgBrush, x, y + 4.0f, w, h - 8.0f);
    g.FillEllipse(&bgBrush, x, y, 8.0f, 8.0f);
    g.FillEllipse(&bgBrush, x + w - 8.0f, y, 8.0f, 8.0f);
    g.FillEllipse(&bgBrush, x, y + h - 8.0f, 8.0f, 8.0f);
    g.FillEllipse(&bgBrush, x + w - 8.0f, y + h - 8.0f, 8.0f, 8.0f);

    // 边框
    if (!m_locked) {
        Gdiplus::Pen border(m_borderColor, m_hovered ? 2.0f : 1.0f);
        g.DrawRectangle(&border, x + 2.0f, y + 2.0f, w - 4.0f, h - 4.0f);
    }

    // 文字
    if (!m_text.empty()) {
        Gdiplus::SolidBrush textBrush(m_locked ? Gdiplus::Color(100, 100, 100, 100)
                                                : m_textColor);
        Gdiplus::StringFormat fmt;
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        Gdiplus::RectF textRect(x, y, w, h);
        g.DrawString(m_text.c_str(), -1, &font, textRect, &fmt, &textBrush);
    }
}
