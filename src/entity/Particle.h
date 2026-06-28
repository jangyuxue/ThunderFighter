#pragma once

#include "entity/Entity.h"
#include <gdiplus.h>

// ============================================================
// 粒子 — 用于爆炸、拖尾等视觉效果
// ============================================================
enum class ParticleKind {
    EXPLOSION,    // 爆炸火花
    SPARK,        // 小火花
    ENGINE_TRAIL, // 引擎拖尾
    SHIELD_RING   // 护盾光环
};

class Particle : public Entity {
public:
    Particle() = default;

    void Init(float x, float y, float vx, float vy,
              ParticleKind kind, Gdiplus::Color color,
              float life = 0.5f, float size = 4.0f);

    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;

    bool IsDead() const { return m_life <= 0.0f; }
    float GetLifeRatio() const { return m_life / m_maxLife; }

private:
    ParticleKind     m_kind   = ParticleKind::EXPLOSION;
    Gdiplus::Color   m_color  = Gdiplus::Color(255, 255, 255);
    float            m_life   = 0.0f;
    float            m_maxLife = 0.5f;
    float            m_size   = 4.0f;
};
