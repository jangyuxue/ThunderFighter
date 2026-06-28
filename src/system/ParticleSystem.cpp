#include "system/ParticleSystem.h"
#include <cmath>
#include <random>

void ParticleSystem::EmitExplosion(float x, float y, float radius, int count) {
    if (!m_pool) return;

    static std::mt19937 rng(std::random_device{}());

    // 橙色→黄色→白色 爆炸粒子
    Gdiplus::Color colors[] = {
        Gdiplus::Color(255, 255, 200, 50),
        Gdiplus::Color(255, 255, 150, 30),
        Gdiplus::Color(255, 255, 100, 20),
        Gdiplus::Color(255, 220, 80,  20),
        Gdiplus::Color(255, 255, 255, 200),
    };

    for (int i = 0; i < count; ++i) {
        Particle* p = m_pool->Acquire();
        if (!p) break;

        float angle = static_cast<float>(rng() % 6283) / 1000.0f;  // 0 to 2π
        float speed = (rng() % 100) / 100.0f * radius * 2.0f + radius * 0.3f;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;
        float life = 0.3f + (rng() % 50) / 100.0f;

        int ci = rng() % 5;
        float size = 2.0f + (rng() % 100) / 100.0f * 5.0f;

        p->Init(x, y, vx, vy, ParticleKind::EXPLOSION, colors[ci], life, size);
    }
}

void ParticleSystem::EmitSparks(float x, float y, int count) {
    if (!m_pool) return;

    static std::mt19937 rng(std::random_device{}());

    for (int i = 0; i < count; ++i) {
        Particle* p = m_pool->Acquire();
        if (!p) break;

        float angle = static_cast<float>(rng() % 6283) / 1000.0f;
        float speed = (rng() % 60) + 40.0f;
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed;
        float life = 0.15f + (rng() % 20) / 100.0f;

        p->Init(x, y, vx, vy, ParticleKind::SPARK,
                Gdiplus::Color(255, 255, 255, 255), life, 2.0f);
    }
}

void ParticleSystem::EmitEngineTrail(float x, float y) {
    if (!m_pool) return;

    Particle* p = m_pool->Acquire();
    if (!p) return;

    static std::mt19937 rng(std::random_device{}());
    float vx = (static_cast<float>(rng() % 30) - 15.0f);
    float vy = (rng() % 10 + 15.0f);
    float life = 0.2f + (rng() % 20) / 100.0f;

    p->Init(x, y, vx, vy, ParticleKind::ENGINE_TRAIL,
            Gdiplus::Color(200, 255, 200, 80), life, 3.0f);
}

void ParticleSystem::EmitBossExplosion(float x, float y) {
    // 大型爆炸，多次发射
    for (int i = 0; i < 5; ++i) {
        EmitExplosion(x + static_cast<float>(rand() % 40 - 20),
                      y + static_cast<float>(rand() % 30 - 15),
                      50.0f, 40);
    }
}

void ParticleSystem::Update(float dt) {
    if (!m_pool) return;

    for (auto& p : *m_pool) {
        p.Update(dt);
    }
}

void ParticleSystem::Render(Gdiplus::Graphics& g) {
    if (!m_pool) return;

    for (auto& p : *m_pool) {
        if (p.IsActive()) {
            p.Render(g);
        }
    }
}
