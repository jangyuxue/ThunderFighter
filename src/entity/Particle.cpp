#include "entity/Particle.h"
#include <cmath>

void Particle::Init(float x, float y, float vx, float vy,
                    ParticleKind kind, Gdiplus::Color color,
                    float life, float size) {
    m_x       = x;
    m_y       = y;
    m_vx      = vx;
    m_vy      = vy;
    m_kind    = kind;
    m_color   = color;
    m_life    = life;
    m_maxLife = life;
    m_size    = size;
    m_active  = true;
}

void Particle::Update(float dt) {
    if (!m_active) return;

    Entity::Update(dt);
    m_life -= dt;

    // 减速效果
    m_vx *= 0.98f;
    m_vy *= 0.98f;

    if (m_life <= 0.0f) {
        Deactivate();
    }
}

void Particle::Render(Gdiplus::Graphics& g) {
    if (!m_active || m_life <= 0.0f) return;

    float ratio = m_life / m_maxLife;
    float alpha = ratio;

    BYTE a = static_cast<BYTE>(m_color.GetA() * alpha);
    Gdiplus::Color fadedColor(a, m_color.GetR(),
                               m_color.GetG(), m_color.GetB());

    float currentSize = m_size * (0.3f + 0.7f * ratio);

    Gdiplus::SolidBrush brush(fadedColor);
    g.FillEllipse(&brush,
                  m_x - currentSize * 0.5f,
                  m_y - currentSize * 0.5f,
                  currentSize, currentSize);
}
