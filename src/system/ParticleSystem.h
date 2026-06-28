#pragma once

#include "entity/Particle.h"
#include "core/ObjectPool.h"
#include "config/GameConfig.h"
#include <gdiplus.h>

// ============================================================
// 粒子系统 — 管理爆炸、火花等视觉特效
// ============================================================
class ParticleSystem {
public:
    ParticleSystem() = default;

    void EmitExplosion(float x, float y, float radius, int count = 30);
    void EmitSparks(float x, float y, int count = 8);
    void EmitEngineTrail(float x, float y);
    void EmitBossExplosion(float x, float y);

    void Update(float dt);
    void Render(Gdiplus::Graphics& g);

    // 设置对象池引用
    void SetPool(ObjectPool<Particle, Config::MAX_PARTICLES>* pool) {
        m_pool = pool;
    }

private:
    ObjectPool<Particle, Config::MAX_PARTICLES>* m_pool = nullptr;
};
