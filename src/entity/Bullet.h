#pragma once

#include "entity/Entity.h"
#include <gdiplus.h>

// ============================================================
// 子弹
// ============================================================
enum class BulletOwner {
    PLAYER,
    ENEMY
};

class Bullet : public Entity {
public:
    Bullet() = default;

    void Init(float x, float y, float vx, float vy,
              int damage, BulletOwner owner, bool isLaser = false);

    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;

    int        GetDamage()   const { return m_damage; }
    BulletOwner GetOwner()   const { return m_owner; }
    bool       IsLaser()     const { return m_isLaser; }

    // 激光的时间管理
    void  SetLaserLife(float life) { m_laserLife = life; }
    float GetLaserLife() const     { return m_laserLife; }
    void  ReduceLaserLife(float dt) { m_laserLife -= dt; }

private:
    int        m_damage   = 1;
    BulletOwner m_owner   = BulletOwner::PLAYER;
    bool       m_isLaser  = false;
    float      m_laserLife = 0.0f;
};
