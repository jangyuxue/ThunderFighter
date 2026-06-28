#pragma once

#include "entity/Entity.h"
#include <gdiplus.h>

// ============================================================
// 道具
// ============================================================
enum class PowerUpType {
    WEAPON_UP,   // 武器升级 (黄)
    SHIELD,      // 护盾 (蓝)
    SPEED_BOOST, // 加速 (绿)
    BOMB         // 炸弹 (红)
};

class PowerUp : public Entity {
public:
    PowerUp() = default;

    void Init(float x, float y, PowerUpType type);
    void Update(float dt) override;
    void Render(Gdiplus::Graphics& g) override;

    PowerUpType GetPowerUpType() const { return m_type; }

    static PowerUpType RandomType();  // 加权随机

private:
    PowerUpType m_type = PowerUpType::WEAPON_UP;
    float       m_animTimer = 0.0f;
};
