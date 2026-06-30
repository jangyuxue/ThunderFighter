#pragma once

#include "entity/Entity.h"
#include <gdiplus.h>
#include <functional>

// ============================================================
// 敌人类型
// ============================================================
enum class EnemyKind {
    SMALL,
    MEDIUM,
    BOSS,
    SHOOTER,   // 射击型：移动快，三向瞄准散射
    TANK,      // 重型坦克：慢速，悬停，八向放射
    ELITE      // 精英机：横向摆动，波浪墙弹幕
};

// 敌人基类
class Enemy : public Entity {
public:
    Enemy() = default;
    virtual ~Enemy() = default;

    virtual void Init(float x, float y, int level);
    virtual void Update(float dt) override;
    virtual void Render(Gdiplus::Graphics& g) override;

    void TakeDamage(int amount);
    bool IsDead()    const { return m_hp <= 0; }
    bool IsDying()   const { return m_state == State::DYING; }

    EnemyKind GetKind()       const { return m_kind; }
    int       GetScoreValue() const { return m_scoreValue; }
    int       GetHP()         const { return m_hp; }
    int       GetMaxHP()      const { return m_maxHP; }
    float     GetDeathTimer() const { return m_deathTimer; }

    // 射击（由 LevelManager 或自身 AI 调用）
    virtual bool ShouldShoot(float dt);
    virtual float GetShootX() const { return m_x; }
    virtual float GetShootY() const { return m_y + m_height * 0.5f; }

    void SetOnDeathCallback(std::function<void(Enemy*)> cb) { m_onDeath = cb; }

protected:
    enum class State { ACTIVE, DYING, DEAD };
    State     m_state      = State::ACTIVE;
    EnemyKind m_kind       = EnemyKind::SMALL;
    int       m_level      = 1;
    int       m_hp         = 1;
    int       m_maxHP      = 1;
    int       m_scoreValue = 100;
    float     m_deathTimer = 0.0f;
    float     m_shootTimer = 0.0f;
    float     m_shootInterval = 2.0f;
    float     m_moveTimer  = 0.0f;

    std::function<void(Enemy*)> m_onDeath;
};

// ---- 小型敌机（快速，之字形移动） ----
class EnemySmall : public Enemy {
public:
    void Init(float x, float y, int level) override;
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;

private:
    float m_zigzagPhase   = 0.0f;
    float m_zigzagSpeed   = 3.0f;
    float m_zigzagAmp     = 60.0f;
    float m_startX        = 0.0f;
};

// ---- 中型敌机（较慢，会射击） ----
class EnemyMedium : public Enemy {
public:
    void Init(float x, float y, int level) override;
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;
    bool ShouldShoot(float dt) override;

private:
    float m_shootChance = 0.5f;    // 每秒射击概率
    float m_hoverTimer  = 0.0f;
    float m_hoverAmp    = 15.0f;
    float m_startY      = 0.0f;
};

// ---- Boss 敌机 ----
class EnemyBoss : public Enemy {
public:
    void Init(float x, float y, int level) override;
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;
    bool ShouldShoot(float dt) override;

    int  GetPhase()    const { return m_phase; }
    bool IsEntering()  const { return m_entering; }

private:
    int   m_phase       = 1;
    bool  m_entering    = true;
    float m_phase2HP;
    float m_phase3HP;
    float m_entranceY   = 80.0f;
    float m_patternTimer = 0.0f;
    int   m_bulletPattern = 0;
};

// ---- 射击型敌机（移动快，三向瞄准散射） ----
class EnemyShooter : public Enemy {
public:
    void Init(float x, float y, int level) override;
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;
    bool ShouldShoot(float dt) override;

private:
    float m_startX = 0.0f;
    float m_swayPhase = 0.0f;
};

// ---- 重型坦克敌机（慢速，悬停，八向放射） ----
class EnemyTank : public Enemy {
public:
    void Init(float x, float y, int level) override;
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;
    bool ShouldShoot(float dt) override;

private:
    float m_startY = 0.0f;
    float m_hoverTimer = 0.0f;
};

// ---- 精英敌机（横向大振幅摆动，波浪墙弹幕） ----
class EnemyElite : public Enemy {
public:
    void Init(float x, float y, int level) override;
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;
    bool ShouldShoot(float dt) override;

private:
    float m_startX = 0.0f;
    float m_swayPhase = 0.0f;
};
