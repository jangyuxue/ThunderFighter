#include "entity/Player.h"

Player::Player() {
    SetSize(static_cast<float>(Config::PLAYER_WIDTH),
            static_cast<float>(Config::PLAYER_HEIGHT));
    SetHitbox(static_cast<float>(Config::PLAYER_HITBOX_W),
              static_cast<float>(Config::PLAYER_HITBOX_H));
}

void Player::Init(PlayerType type) {
    m_type       = type;
    m_lives      = Config::PLAYER_START_LIVES;
    m_maxLives   = Config::PLAYER_MAX_LIVES;
    m_bombs      = 2;
    m_shields    = 0;
    m_weaponLevel = 0;
    m_invincibleTimer = 0.0f;
    m_speedBoostTimer = 0.0f;
    m_shootCooldown   = 0.0f;

    switch (type) {
    case PlayerType::SPEED:
        m_speed     = Config::PLAYER_SPEED_FAST;
        m_baseSpeed = Config::PLAYER_SPEED_FAST;
        break;
    case PlayerType::POWER:
        m_speed     = Config::PLAYER_SPEED_POWER;
        m_baseSpeed = Config::PLAYER_SPEED_POWER;
        m_weaponLevel = 1;  // 力量型初始双发
        break;
    case PlayerType::BALANCED:
    default:
        m_speed     = Config::PLAYER_SPEED_BALANCED;
        m_baseSpeed = Config::PLAYER_SPEED_BALANCED;
        break;
    }

    // 初始位置：屏幕底部中央
    m_x = Config::CANVAS_WIDTH * 0.5f;
    m_y = Config::CANVAS_HEIGHT - 60.0f;
    m_active = true;
}

float Player::GetShootInterval() const {
    switch (m_weaponLevel) {
    case 0: return Config::SHOOT_COOLDOWN_SINGLE;
    case 1: return Config::SHOOT_COOLDOWN_DOUBLE;
    case 2: return Config::SHOOT_COOLDOWN_DOUBLE;
    case 3: return Config::SHOOT_COOLDOWN_SPREAD;
    case 4: return Config::SHOOT_COOLDOWN_LASER;
    default: return Config::SHOOT_COOLDOWN_SINGLE;
    }
}

void Player::Update(float dt, float dx, float dy, bool /*shooting*/) {
    // 移动
    float moveX = dx;
    float moveY = dy;

    // 归一化对角移动
    if (moveX != 0.0f && moveY != 0.0f) {
        const float invSqrt2 = 0.7071f;
        moveX *= invSqrt2;
        moveY *= invSqrt2;
    }

    m_x += moveX * m_speed * dt * 60.0f;
    m_y += moveY * m_speed * dt * 60.0f;

    // 边界限制
    float halfW = m_hitboxW * 0.5f;
    float halfH = m_hitboxH * 0.5f;
    if (m_x - halfW < 0) m_x = halfW;
    if (m_x + halfW > Config::CANVAS_WIDTH) m_x = Config::CANVAS_WIDTH - halfW;
    if (m_y - halfH < 0) m_y = halfH;
    if (m_y + halfH > Config::CANVAS_HEIGHT) m_y = Config::CANVAS_HEIGHT - halfH;

    // 射击冷却
    if (m_shootCooldown > 0.0f) {
        m_shootCooldown -= dt;
    }

    // 无敌计时器
    if (m_invincibleTimer > 0.0f) {
        m_invincibleTimer -= dt;
    }

    // 加速计时器
    if (m_speedBoostTimer > 0.0f) {
        m_speedBoostTimer -= dt;
        if (m_speedBoostTimer <= 0.0f) {
            m_speed = m_baseSpeed;
        }
    }

    // 动画计时器
    m_animTimer += dt;
}

void Player::TakeDamage(int amount) {
    if (m_invincibleTimer > 0.0f) return;

    // 先消耗护盾
    if (m_shields > 0) {
        --m_shields;
        m_invincibleTimer = Config::PLAYER_INVINCIBLE_TIME;
        return;
    }

    m_lives -= amount;
    if (m_lives < 0) m_lives = 0;

    if (m_lives > 0) {
        m_invincibleTimer = Config::PLAYER_INVINCIBLE_TIME;
        m_shootCooldown = 0.0f;   // 重置射击冷却，防止卡住
        if (m_weaponLevel > 0) --m_weaponLevel;
    } else {
        Deactivate();  // 死亡时停用实体
    }
}

void Player::AddExtraLife() {
    if (m_lives < m_maxLives) ++m_lives;
}

void Player::AddShield() {
    if (m_shields < 3) ++m_shields;
}

void Player::AddBomb() {
    if (m_bombs < 5) ++m_bombs;
}

void Player::UseBomb() {
    if (m_bombs > 0) --m_bombs;
}

void Player::ActivateSpeedBoost() {
    m_speed = m_baseSpeed * Config::SPEED_BOOST_MULTIPLIER;
    m_speedBoostTimer = Config::SPEED_BOOST_DURATION;
}

void Player::UpgradeWeapon() {
    if (m_weaponLevel < Config::MAX_WEAPON_LEVEL) {
        ++m_weaponLevel;
    }
}

void Player::ApplyPermanentSpeed() {
    // 永久加速：直接提升基础速度（非临时计时器），整局生效
    m_baseSpeed *= Config::SPEED_BOOST_MULTIPLIER;
    m_speed = m_baseSpeed;
}

void Player::IncreaseMaxLives() {
    // 最大生命+1：提升上限并补1生命
    ++m_maxLives;
    if (m_lives < m_maxLives) ++m_lives;
}

void Player::Render(Gdiplus::Graphics& g) {
    // 无敌闪烁效果
    if (m_invincibleTimer > 0.0f) {
        m_blinkCounter++;
        if ((m_blinkCounter / 4) % 2 == 0) return;
    }

    // 飞机颜色
    Gdiplus::Color bodyColor;
    Gdiplus::Color wingColor;
    Gdiplus::Color cockpitColor(100, 220, 255);

    switch (m_type) {
    case PlayerType::SPEED:
        bodyColor  = Gdiplus::Color(80,  160, 255);
        wingColor  = Gdiplus::Color(40,  120, 220);
        break;
    case PlayerType::POWER:
        bodyColor  = Gdiplus::Color(255, 60,  60);
        wingColor  = Gdiplus::Color(200, 30,  30);
        break;
    case PlayerType::BALANCED:
    default:
        bodyColor  = Gdiplus::Color(60,  220, 80);
        wingColor  = Gdiplus::Color(30,  170, 50);
        break;
    }

    // 引擎火焰（位于飞机底部）
    float flameFlicker = 0.7f + 0.3f * sin(m_animTimer * 20.0f);
    Gdiplus::Color flameOuter(255, 180, 40, static_cast<BYTE>(180 * flameFlicker));
    Gdiplus::Color flameInner(255, 255, 200, static_cast<BYTE>(200 * flameFlicker));

    Gdiplus::SolidBrush flameOuterBrush(flameOuter);
    Gdiplus::SolidBrush flameInnerBrush(flameInner);
    g.FillEllipse(&flameOuterBrush, m_x - 5.0f, m_y + 16.0f, 10.0f, 10.0f);
    g.FillEllipse(&flameInnerBrush, m_x - 3.0f, m_y + 18.0f, 6.0f,  6.0f);

    // 机身主体
    Gdiplus::SolidBrush bodyBrush(bodyColor);
    Gdiplus::PointF bodyPts[3] = {
        { m_x,       m_y - 20 },
        { m_x - 14,  m_y + 8  },
        { m_x + 14,  m_y + 8  }
    };
    g.FillPolygon(&bodyBrush, bodyPts, 3);

    // 机翼
    Gdiplus::SolidBrush wingBrush(wingColor);
    Gdiplus::PointF wingLPts[3] = {
        { m_x - 8,  m_y - 6  },
        { m_x - 18, m_y + 4  },
        { m_x - 8,  m_y + 2  }
    };
    Gdiplus::PointF wingRPts[3] = {
        { m_x + 8,  m_y - 6  },
        { m_x + 18, m_y + 4  },
        { m_x + 8,  m_y + 2  }
    };
    g.FillPolygon(&wingBrush, wingLPts, 3);
    g.FillPolygon(&wingBrush, wingRPts, 3);

    // 座舱
    Gdiplus::SolidBrush cockpitBrush(cockpitColor);
    g.FillEllipse(&cockpitBrush, m_x - 4.0f, m_y - 14.0f, 8.0f, 9.0f);

    // 尾翼
    Gdiplus::PointF tailPts[3] = {
        { m_x,       m_y + 2  },
        { m_x - 8,   m_y + 12 },
        { m_x + 8,   m_y + 12 }
    };
    Gdiplus::SolidBrush tailBrush(wingColor);
    g.FillPolygon(&tailBrush, tailPts, 3);

    // 护盾效果
    if (m_shields > 0) {
        Gdiplus::Color shieldColor;
        if (m_shields >= 3) shieldColor = Gdiplus::Color(180, 50, 220, 255);
        else if (m_shields == 2) shieldColor = Gdiplus::Color(180, 50, 180, 255);
        else shieldColor = Gdiplus::Color(180, 100, 200, 255);

        float pulse = 1.0f + 0.1f * sin(m_animTimer * 4.0f);
        float r = 22.0f * pulse;
        Gdiplus::Pen shieldPen(shieldColor, 2.5f);
        g.DrawEllipse(&shieldPen, m_x - r, m_y - r, r * 2, r * 2);

        // 更透明内圈
        Gdiplus::Color shieldInner(80, shieldColor.GetR(),
                                    shieldColor.GetG(), shieldColor.GetB());
        Gdiplus::Pen shieldInnerPen(shieldInner, 1.5f);
        g.DrawEllipse(&shieldInnerPen, m_x - r * 0.75f, m_y - r * 0.75f,
                      r * 1.5f, r * 1.5f);
    }
}
