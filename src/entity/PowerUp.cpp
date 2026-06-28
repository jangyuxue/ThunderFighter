#include "entity/PowerUp.h"
#include <random>

void PowerUp::Init(float x, float y, PowerUpType type) {
    m_x     = x;
    m_y     = y;
    m_vx    = 0.0f;
    m_vy    = Config::POWERUP_SPEED;
    m_type  = type;
    m_active = true;
    m_animTimer = 0.0f;
    SetSize(20.0f, 20.0f);
    SetHitbox(18.0f, 18.0f);
}

void PowerUp::Update(float dt) {
    if (!m_active) return;

    Entity::Update(dt);
    m_animTimer += dt;

    if (m_y > Config::CANVAS_HEIGHT + 30.0f) {
        Deactivate();
    }
}

PowerUpType PowerUp::RandomType() {
    static std::mt19937 rng(std::random_device{}());
    int roll = rng() % 100;

    // 加权随机: 武器40%, 护盾25%, 加速20%, 炸弹15%
    if (roll < 40)  return PowerUpType::WEAPON_UP;
    if (roll < 65)  return PowerUpType::SHIELD;
    if (roll < 85)  return PowerUpType::SPEED_BOOST;
    return PowerUpType::BOMB;
}

void PowerUp::Render(Gdiplus::Graphics& g) {
    if (!m_active) return;

    Gdiplus::Color color;
    wchar_t label;

    switch (m_type) {
    case PowerUpType::WEAPON_UP:
        color = Gdiplus::Color(255, 240, 80);
        label = L'W';
        break;
    case PowerUpType::SHIELD:
        color = Gdiplus::Color(255, 100, 200, 255);
        label = L'S';
        break;
    case PowerUpType::SPEED_BOOST:
        color = Gdiplus::Color(255, 80, 220, 80);
        label = L'B';
        break;
    case PowerUpType::BOMB:
        color = Gdiplus::Color(255, 255, 80, 80);
        label = L'X';
        break;
    }

    // 背景光圈
    float pulse = 1.0f + 0.1f * sin(m_animTimer * 4.0f);
    float r = 10.0f * pulse;

    Gdiplus::Color glow(80, color.GetR(), color.GetG(), color.GetB());
    Gdiplus::SolidBrush glowBrush(glow);
    g.FillEllipse(&glowBrush, m_x - r - 2, m_y - r - 2, (r + 2) * 2, (r + 2) * 2);

    // 主体圆
    Gdiplus::SolidBrush brush(color);
    g.FillEllipse(&brush, m_x - r, m_y - r, r * 2, r * 2);

    // 文字标签
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::Font font(L"Arial", 11, Gdiplus::FontStyleBold);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::RectF rect(m_x - r, m_y - r, r * 2, r * 2);
    g.DrawString(&label, 1, &font, rect, &fmt, &textBrush);
}
