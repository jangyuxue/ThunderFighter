#pragma once

#include "entity/Entity.h"
#include "config/GameConfig.h"

// ============================================================
// 玩家飞机
// ============================================================
enum class PlayerType {
    SPEED,    // 速度型 — 蓝
    POWER,    // 力量型 — 红
    BALANCED  // 均衡型 — 绿
};

class Player : public Entity {
public:
    Player();

    void Init(PlayerType type);

    // 更新（dt 为秒）
    void Update(float dt, float dx, float dy, bool shooting);

    // 受击
    void TakeDamage(int amount);

    // 道具效果
    void AddShield();
    void AddBomb();
    void UseBomb();         // 消耗一个炸弹
    void ActivateSpeedBoost();
    void UpgradeWeapon();

    using Entity::Update;   // 避免隐藏基类虚函数

    // 获取状态
    PlayerType GetType()        const { return m_type; }
    int        GetLives()       const { return m_lives; }
    int        GetBombs()       const { return m_bombs; }
    int        GetShields()     const { return m_shields; }
    int        GetWeaponLevel() const { return m_weaponLevel; }
    float      GetSpeed()       const { return m_speed; }
    bool       IsInvincible()   const { return m_invincibleTimer > 0.0f; }
    bool       HasSpeedBoost()  const { return m_speedBoostTimer > 0.0f; }

    // 获取开火位置（用于武器系统创建子弹）
    float GetFirePosX() const { return m_x; }
    float GetFirePosY() const { return m_y - m_height * 0.5f; }

    // 刷新状态（被 WeaponSystem 查询）
    bool   IsAlive() const { return m_lives > 0; }
    float& GetShootCooldown() { return m_shootCooldown; }
    float  GetShootInterval() const;

    virtual void Render(Gdiplus::Graphics& g) override;

private:
    PlayerType m_type = PlayerType::BALANCED;
    int   m_lives       = Config::PLAYER_START_LIVES;
    int   m_bombs       = 2;
    int   m_shields     = 0;
    int   m_weaponLevel = 0;
    float m_speed       = Config::PLAYER_SPEED_BALANCED;
    float m_baseSpeed   = Config::PLAYER_SPEED_BALANCED;

    float m_invincibleTimer = 0.0f;
    float m_speedBoostTimer = 0.0f;
    float m_shootCooldown   = 0.0f;

    // 视觉效果
    float m_animTimer   = 0.0f;
    int   m_blinkCounter = 0;
};
