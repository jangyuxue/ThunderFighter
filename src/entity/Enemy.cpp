#include "entity/Enemy.h"
#include <cmath>
#include <cstdio>

// ============================================================
// Enemy 基类
// ============================================================
void Enemy::Init(float x, float y, int level) {
    m_x = x;
    m_y = y;
    m_level = level;
    m_state = State::ACTIVE;
    m_active = true;
    m_deathTimer = 0.0f;
    m_shootTimer = 0.0f;
}

void Enemy::Update(float dt) {
    if (m_state == State::DYING) {
        m_deathTimer += dt;
        float deathDuration = (m_kind == EnemyKind::BOSS) ? 0.6f : 0.3f;
        if (m_deathTimer > deathDuration) {
            m_state = State::DEAD;
            m_active = false;
        }
        return;
    }

    if (m_state != State::ACTIVE) return;

    Entity::Update(dt);

    // 超出屏幕销毁
    if (m_y > Config::CANVAS_HEIGHT + 60.0f) {
        Deactivate();
        m_state = State::DEAD;
    }

    m_shootTimer += dt;
    m_moveTimer += dt;
}

void Enemy::TakeDamage(int amount) {
    if (m_state != State::ACTIVE) return;

    m_hp -= amount;
    if (m_hp <= 0) {
        m_hp = 0;
        m_state = State::DYING;
        m_deathTimer = 0.0f;
        if (m_onDeath) {
            m_onDeath(this);
        }
    }
}

bool Enemy::ShouldShoot(float /*dt*/) {
    return false;  // 基类不射击
}

void Enemy::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        // 死亡闪烁效果
        float alpha = 1.0f - (m_deathTimer / 0.3f);
        Gdiplus::Color flicker(static_cast<BYTE>(255 * alpha), 255, 100, 50);
        Gdiplus::SolidBrush flickerBrush(flicker);
        g.FillEllipse(&flickerBrush, m_x - 15.0f, m_y - 15.0f, 30.0f, 30.0f);
        return;
    }

    // 默认绘制为红色菱形
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 80, 80));
    Gdiplus::PointF pts[4] = {
        { m_x,       m_y - 12 },
        { m_x + 10,  m_y      },
        { m_x,       m_y + 12 },
        { m_x - 10,  m_y      }
    };
    g.FillPolygon(&brush, pts, 4);
}

// ============================================================
// EnemySmall — 小型快速敌机（之字形）
// ============================================================
void EnemySmall::Init(float x, float y, int level) {
    Enemy::Init(x, y, level);
    m_kind = EnemyKind::SMALL;
    m_hp   = Config::ENEMY_SMALL_HP + level - 1;
    m_maxHP = m_hp;
    m_scoreValue = Config::SCORE_SMALL_ENEMY;
    m_vy  = Config::ENEMY_SMALL_SPEED;
    m_zigzagPhase = static_cast<float>(rand()) / RAND_MAX * 6.28f;
    m_startX = x;
    m_zigzagAmp = 50.0f + level * 10.0f;
    SetSize(24.0f, 24.0f);
    SetHitbox(18.0f, 18.0f);
}

void EnemySmall::Update(float dt) {
    if (m_state != State::ACTIVE) {
        Enemy::Update(dt);
        return;
    }

    // 之字形移动
    m_zigzagPhase += dt * m_zigzagSpeed * 3.0f;
    m_x = m_startX + sin(m_zigzagPhase) * m_zigzagAmp;

    // Y 轴向下
    m_y += m_vy * dt * 60.0f;

    if (m_y > Config::CANVAS_HEIGHT + 60.0f) {
        Deactivate();
        m_state = State::DEAD;
    }
}

void EnemySmall::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        float alpha = 1.0f - (m_deathTimer / 0.3f);
        Gdiplus::Color flicker(static_cast<BYTE>(220 * alpha), 255, 150, 50);
        Gdiplus::SolidBrush fb(flicker);
        g.FillEllipse(&fb, m_x - 12.0f, m_y - 12.0f, 24.0f, 24.0f);
        return;
    }

    // 棕色小飞机
    Gdiplus::SolidBrush body(Gdiplus::Color(220, 180, 80));
    Gdiplus::PointF bodyPts[3] = {
        { m_x,       m_y - 10 },
        { m_x - 9,   m_y + 6  },
        { m_x + 9,   m_y + 6  }
    };
    g.FillPolygon(&body, bodyPts, 3);

    // 翅膀
    Gdiplus::SolidBrush wing(Gdiplus::Color(200, 140, 50));
    Gdiplus::PointF wingL[3] = { { m_x - 4,  m_y - 2 }, { m_x - 12, m_y + 3 }, { m_x - 4, m_y + 0 } };
    Gdiplus::PointF wingR[3] = { { m_x + 4,  m_y - 2 }, { m_x + 12, m_y + 3 }, { m_x + 4, m_y + 0 } };
    g.FillPolygon(&wing, wingL, 3);
    g.FillPolygon(&wing, wingR, 3);
}

// ============================================================
// EnemyMedium — 中型敌机（会射击）
// ============================================================
void EnemyMedium::Init(float x, float y, int level) {
    Enemy::Init(x, y, level);
    m_kind = EnemyKind::MEDIUM;
    m_hp   = Config::ENEMY_MEDIUM_HP + level * 2;
    m_maxHP = m_hp;
    m_scoreValue = Config::SCORE_MEDIUM_ENEMY;
    m_vy  = Config::ENEMY_MEDIUM_SPEED;
    m_hoverTimer = 0.0f;
    m_startY = y;
    m_shootInterval = 2.0f / (0.8f + level * 0.3f);
    SetSize(32.0f, 36.0f);
    SetHitbox(26.0f, 28.0f);
}

void EnemyMedium::Update(float dt) {
    if (m_state != State::ACTIVE) {
        Enemy::Update(dt);
        return;
    }

    // 缓慢下降 + 悬停浮动
    m_y += m_vy * dt * 60.0f;
    m_hoverTimer += dt;
    m_x += sin(m_hoverTimer * 2.0f) * dt * 20.0f;

    // 到达一定高度后停止下降
    if (m_y > 180.0f) {
        m_vy = 0.0f;
    }

    m_shootTimer += dt;

    if (m_y > Config::CANVAS_HEIGHT + 60.0f) {
        Deactivate();
        m_state = State::DEAD;
    }
}

bool EnemyMedium::ShouldShoot(float /*dt*/) {
    if (m_y < 50.0f) return false;  // 还没完全入场
    if (m_shootTimer < m_shootInterval) return false;

    // 概率性射击
    float roll = static_cast<float>(rand()) / RAND_MAX;
    if (roll < m_shootChance) {
        m_shootTimer = 0.0f;
        return true;
    }
    return false;
}

void EnemyMedium::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        float alpha = 1.0f - (m_deathTimer / 0.3f);
        Gdiplus::Color flicker(static_cast<BYTE>(200 * alpha), 200, 200, 200);
        Gdiplus::SolidBrush fb(flicker);
        g.FillEllipse(&fb, m_x - 16.0f, m_y - 18.0f, 32.0f, 36.0f);
        return;
    }

    // 灰色中型飞机
    Gdiplus::SolidBrush body(Gdiplus::Color(180, 180, 200));
    Gdiplus::PointF pts[4] = {
        { m_x,       m_y - 16 },
        { m_x + 12,  m_y + 4  },
        { m_x,       m_y + 14 },
        { m_x - 12,  m_y + 4  }
    };
    g.FillPolygon(&body, pts, 4);

    // 机翼
    Gdiplus::SolidBrush wing(Gdiplus::Color(140, 140, 170));
    Gdiplus::PointF wl[3] = { { m_x - 6, m_y - 4 }, { m_x - 16, m_y + 6 }, { m_x - 6, m_y + 4 } };
    Gdiplus::PointF wr[3] = { { m_x + 6, m_y - 4 }, { m_x + 16, m_y + 6 }, { m_x + 6, m_y + 4 } };
    g.FillPolygon(&wing, wl, 3);
    g.FillPolygon(&wing, wr, 3);

    // 血条（仅在受伤时显示）
    if (m_hp < m_maxHP) {
        float barW = 24.0f;
        float barH = 3.0f;
        float barX = m_x - barW * 0.5f;
        float barY = m_y - 22.0f;

        Gdiplus::SolidBrush bgBar(Gdiplus::Color(60, 60, 60));
        g.FillRectangle(&bgBar, barX, barY, barW, barH);

        float ratio = static_cast<float>(m_hp) / m_maxHP;
        Gdiplus::SolidBrush hpBar(Gdiplus::Color(200, 220, 60));
        g.FillRectangle(&hpBar, barX, barY, barW * ratio, barH);
    }
}

// ============================================================
// EnemyBoss — Boss 敌机
// ============================================================
void EnemyBoss::Init(float x, float y, int level) {
    Enemy::Init(x, y, level);
    m_kind  = EnemyKind::BOSS;
    m_hp    = Config::BOSS_BASE_HP + level * 50;
    m_maxHP = m_hp;
    m_scoreValue = Config::SCORE_BOSS;
    m_vy = Config::BOSS_SPEED;
    m_entering = true;
    m_entranceY = 80.0f;
    m_phase = 1;
    m_phase2HP = m_maxHP * 0.66f;
    m_phase3HP = m_maxHP * 0.33f;
    m_shootInterval = 0.8f;
    m_patternTimer = 0.0f;
    m_bulletPattern = 0;
    SetSize(60.0f, 50.0f);
    SetHitbox(50.0f, 40.0f);
}

void EnemyBoss::Update(float dt) {
    if (m_state != State::ACTIVE) {
        Enemy::Update(dt);
        return;
    }

    // 入场动画
    if (m_entering) {
        m_y += dt * 40.0f;
        if (m_y >= m_entranceY) {
            m_y = m_entranceY;
            m_entering = false;
        }
        return;
    }

    // Boss 左右飘移
    m_moveTimer += dt;
    m_x += sin(m_moveTimer * 1.5f) * dt * 30.0f;
    if (m_x - 30 < 0) m_x = 30;
    if (m_x + 30 > Config::CANVAS_WIDTH) m_x = Config::CANVAS_WIDTH - 30;

    m_shootTimer += dt;
    m_patternTimer += dt;

    // 阶段切换
    if (m_hp <= m_phase3HP && m_phase == 2) {
        m_phase = 3;
        m_shootInterval = 0.3f;
        m_patternTimer = 0.0f;
    } else if (m_hp <= m_phase2HP && m_phase == 1) {
        m_phase = 2;
        m_shootInterval = 0.5f;
        m_patternTimer = 0.0f;
    }
}

bool EnemyBoss::ShouldShoot(float /*dt*/) {
    if (m_entering) return false;
    if (m_shootTimer < m_shootInterval) return false;

    m_shootTimer = 0.0f;

    // 每 2 秒切换弹幕模式
    if (m_patternTimer > 2.0f) {
        m_patternTimer = 0.0f;
        m_bulletPattern = (m_bulletPattern + 1) % (m_phase + 2);
    }

    return true;
}

void EnemyBoss::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        // Boss 死亡大爆炸效果
        float t = m_deathTimer / 0.6f;
        float r = 20.0f + t * 60.0f;
        float alpha = 1.0f - t;
        Gdiplus::Color flame(static_cast<BYTE>(200 * alpha), 255,
                             static_cast<BYTE>(200 * (1 - t)),
                             static_cast<BYTE>(50 * (1 - t)));
        Gdiplus::SolidBrush fb(flame);
        g.FillEllipse(&fb, m_x - r, m_y - r, r * 2, r * 2);
        return;
    }

    // Boss 大型敌机外观
    // 主体
    Gdiplus::SolidBrush bodyBrush(Gdiplus::Color(180, 50, 60));
    Gdiplus::PointF bodyPts[6] = {
        { m_x,       m_y - 25 },
        { m_x + 20,  m_y - 8  },
        { m_x + 20,  m_y + 12 },
        { m_x,       m_y + 22 },
        { m_x - 20,  m_y + 12 },
        { m_x - 20,  m_y - 8  }
    };
    g.FillPolygon(&bodyBrush, bodyPts, 6);

    // 装甲板
    Gdiplus::SolidBrush armorBrush(Gdiplus::Color(140, 30, 40));
    Gdiplus::PointF armorPts[4] = {
        { m_x,       m_y - 15 },
        { m_x + 15,  m_y - 3  },
        { m_x,       m_y + 8  },
        { m_x - 15,  m_y - 3  }
    };
    g.FillPolygon(&armorBrush, armorPts, 4);

    // 核心（随阶段变色）
    Gdiplus::Color coreColor;
    switch (m_phase) {
    case 1: coreColor = Gdiplus::Color(255, 255, 100); break;
    case 2: coreColor = Gdiplus::Color(255, 255, 50,  50);  break;
    case 3: coreColor = Gdiplus::Color(255, 255, 0,   0);   break;
    }
    float pulse = 1.0f + 0.15f * sin(m_moveTimer * 5.0f);
    Gdiplus::SolidBrush coreBrush(coreColor);
    g.FillEllipse(&coreBrush, m_x - 8 * pulse, m_y - 8 * pulse,
                  16 * pulse, 16 * pulse);

    // Boss 血条（始终显示）
    float barW = 200.0f;
    float barH = 8.0f;
    float barX = Config::CANVAS_WIDTH * 0.5f - barW * 0.5f;
    float barY = 15.0f;

    // 背景
    Gdiplus::SolidBrush bgBar(Gdiplus::Color(40, 40, 40));
    g.FillRectangle(&bgBar, barX, barY, barW, barH);

    // 血量
    float ratio = static_cast<float>(m_hp) / m_maxHP;
    Gdiplus::Color hpColor;
    if (ratio > 0.5f) hpColor = Gdiplus::Color(220, 220, 60);
    else if (ratio > 0.25f) hpColor = Gdiplus::Color(220, 140, 30);
    else hpColor = Gdiplus::Color(220, 40, 30);
    Gdiplus::SolidBrush hpBar(hpColor);
    g.FillRectangle(&hpBar, barX, barY, barW * ratio, barH);

    // 边框
    Gdiplus::Pen barPen(Gdiplus::Color(180, 180, 180), 1.5f);
    g.DrawRectangle(&barPen, barX, barY, barW, barH);

    // Boss 名称
    // 阶段标记
    Gdiplus::SolidBrush phaseBrush(Gdiplus::Color(200, 255, 255, 100));
    Gdiplus::Font font(L"Arial", 10);
    wchar_t buf[32];
    swprintf(buf, 32, L"PHASE %d", m_phase);
    g.DrawString(buf, -1, &font, Gdiplus::PointF(barX, barY + 12), &phaseBrush);
}

// ============================================================
// EnemyShooter — 射击型敌机（快速，三向瞄准散射）
// ============================================================
void EnemyShooter::Init(float x, float y, int level) {
    Enemy::Init(x, y, level);
    m_kind = EnemyKind::SHOOTER;
    m_hp   = Config::ENEMY_SHOOTER_HP + level;
    m_maxHP = m_hp;
    m_scoreValue = Config::SCORE_SHOOTER_ENEMY;
    m_vy  = Config::ENEMY_SHOOTER_SPEED;
    m_startX = x;
    m_swayPhase = static_cast<float>(rand()) / RAND_MAX * 6.28f;
    m_shootInterval = 1.6f / (0.7f + level * 0.15f);
    SetSize(28.0f, 28.0f);
    SetHitbox(20.0f, 20.0f);
}

void EnemyShooter::Update(float dt) {
    if (m_state != State::ACTIVE) {
        Enemy::Update(dt);
        return;
    }

    // 边下降边快速横向摇摆
    m_swayPhase += dt * 5.0f;
    m_x = m_startX + sin(m_swayPhase) * (40.0f + m_level * 8.0f);
    m_y += m_vy * dt * 60.0f;

    m_shootTimer += dt;

    if (m_y > Config::CANVAS_HEIGHT + 60.0f) {
        Deactivate();
        m_state = State::DEAD;
    }
}

bool EnemyShooter::ShouldShoot(float /*dt*/) {
    if (m_y < 60.0f) return false;
    if (m_shootTimer < m_shootInterval) return false;
    m_shootTimer = 0.0f;
    return true;
}

void EnemyShooter::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        float alpha = 1.0f - (m_deathTimer / 0.3f);
        Gdiplus::Color flicker(static_cast<BYTE>(220 * alpha), 255, 200, 50);
        Gdiplus::SolidBrush fb(flicker);
        g.FillEllipse(&fb, m_x - 14.0f, m_y - 14.0f, 28.0f, 28.0f);
        return;
    }

    // 橙色攻击性机身
    Gdiplus::SolidBrush body(Gdiplus::Color(240, 140, 60));
    Gdiplus::PointF bodyPts[3] = {
        { m_x,       m_y - 12 },
        { m_x - 10,  m_y + 8  },
        { m_x + 10,  m_y + 8  }
    };
    g.FillPolygon(&body, bodyPts, 3);

    // 绿色机翼
    Gdiplus::SolidBrush wing(Gdiplus::Color(120, 220, 80));
    Gdiplus::PointF wl[3] = { { m_x - 5, m_y - 2 }, { m_x - 18, m_y + 6 }, { m_x - 5, m_y + 4 } };
    Gdiplus::PointF wr[3] = { { m_x + 5, m_y - 2 }, { m_x + 18, m_y + 6 }, { m_x + 5, m_y + 4 } };
    g.FillPolygon(&wing, wl, 3);
    g.FillPolygon(&wing, wr, 3);
}

// ============================================================
// EnemyTank — 重型坦克敌机（慢速，悬停，八向放射）
// ============================================================
void EnemyTank::Init(float x, float y, int level) {
    Enemy::Init(x, y, level);
    m_kind = EnemyKind::TANK;
    m_hp   = Config::ENEMY_TANK_HP + level * 3;
    m_maxHP = m_hp;
    m_scoreValue = Config::SCORE_TANK_ENEMY;
    m_vy  = Config::ENEMY_TANK_SPEED;
    m_startY = y;
    m_hoverTimer = 0.0f;
    m_shootInterval = 3.0f / (0.6f + level * 0.15f);
    SetSize(40.0f, 36.0f);
    SetHitbox(32.0f, 30.0f);
}

void EnemyTank::Update(float dt) {
    if (m_state != State::ACTIVE) {
        Enemy::Update(dt);
        return;
    }

    // 缓慢下降，到达 y>160 后悬停
    m_y += m_vy * dt * 60.0f;
    if (m_y > 160.0f) {
        m_vy = 0.0f;
        m_hoverTimer += dt;
        m_x += sin(m_hoverTimer * 1.2f) * dt * 12.0f;
    }

    m_shootTimer += dt;

    if (m_y > Config::CANVAS_HEIGHT + 60.0f) {
        Deactivate();
        m_state = State::DEAD;
    }
}

bool EnemyTank::ShouldShoot(float /*dt*/) {
    if (m_y < 80.0f) return false;
    if (m_shootTimer < m_shootInterval) return false;
    m_shootTimer = 0.0f;
    return true;
}

void EnemyTank::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        float alpha = 1.0f - (m_deathTimer / 0.3f);
        Gdiplus::Color flicker(static_cast<BYTE>(200 * alpha), 150, 150, 150);
        Gdiplus::SolidBrush fb(flicker);
        g.FillEllipse(&fb, m_x - 20.0f, m_y - 18.0f, 40.0f, 36.0f);
        return;
    }

    // 深色厚重装甲
    Gdiplus::SolidBrush body(Gdiplus::Color(90, 90, 110));
    Gdiplus::PointF pts[6] = {
        { m_x,       m_y - 18 },
        { m_x + 20,  m_y - 6  },
        { m_x + 20,  m_y + 10 },
        { m_x,       m_y + 18 },
        { m_x - 20,  m_y + 10 },
        { m_x - 20,  m_y - 6  }
    };
    g.FillPolygon(&body, pts, 6);

    // 红色核心
    float pulse = 1.0f + 0.1f * sin(m_hoverTimer * 3.0f);
    Gdiplus::SolidBrush core(Gdiplus::Color(255, 80, 40));
    g.FillEllipse(&core, m_x - 8 * pulse, m_y - 6 * pulse,
                 16 * pulse, 12 * pulse);

    // 血条（始终显示，因为血量高）
    float barW = 30.0f, barH = 3.0f;
    float barX = m_x - barW * 0.5f, barY = m_y - 24.0f;
    Gdiplus::SolidBrush bgBar(Gdiplus::Color(60, 60, 60));
    g.FillRectangle(&bgBar, barX, barY, barW, barH);
    float ratio = static_cast<float>(m_hp) / m_maxHP;
    Gdiplus::SolidBrush hpBar(Gdiplus::Color(220, 60, 40));
    g.FillRectangle(&hpBar, barX, barY, barW * ratio, barH);
}

// ============================================================
// EnemyElite — 精英敌机（横向大振幅摆动，波浪墙弹幕）
// ============================================================
void EnemyElite::Init(float x, float y, int level) {
    Enemy::Init(x, y, level);
    m_kind = EnemyKind::ELITE;
    m_hp   = Config::ENEMY_ELITE_HP + level * 2;
    m_maxHP = m_hp;
    m_scoreValue = Config::SCORE_ELITE_ENEMY;
    m_vy  = Config::ENEMY_ELITE_SPEED;
    m_startX = x;
    m_swayPhase = static_cast<float>(rand()) / RAND_MAX * 6.28f;
    m_shootInterval = 2.2f / (0.7f + level * 0.15f);
    SetSize(34.0f, 34.0f);
    SetHitbox(26.0f, 26.0f);
}

void EnemyElite::Update(float dt) {
    if (m_state != State::ACTIVE) {
        Enemy::Update(dt);
        return;
    }

    // 大振幅横向正弦波下降
    m_swayPhase += dt * 2.5f;
    m_x = m_startX + sin(m_swayPhase) * (70.0f + m_level * 10.0f);
    m_y += m_vy * dt * 60.0f;

    m_shootTimer += dt;

    if (m_y > Config::CANVAS_HEIGHT + 60.0f) {
        Deactivate();
        m_state = State::DEAD;
    }
}

bool EnemyElite::ShouldShoot(float /*dt*/) {
    if (m_y < 70.0f) return false;
    if (m_shootTimer < m_shootInterval) return false;
    m_shootTimer = 0.0f;
    return true;
}

void EnemyElite::Render(Gdiplus::Graphics& g) {
    if (!m_active && m_state == State::DEAD) return;

    if (m_state == State::DYING) {
        float alpha = 1.0f - (m_deathTimer / 0.3f);
        Gdiplus::Color flicker(static_cast<BYTE>(220 * alpha), 200, 80, 220);
        Gdiplus::SolidBrush fb(flicker);
        g.FillEllipse(&fb, m_x - 17.0f, m_y - 17.0f, 34.0f, 34.0f);
        return;
    }

    // 紫色精英机身
    Gdiplus::SolidBrush body(Gdiplus::Color(180, 80, 220));
    Gdiplus::PointF pts[4] = {
        { m_x,       m_y - 15 },
        { m_x + 14,  m_y + 2  },
        { m_x,       m_y + 15 },
        { m_x - 14,  m_y + 2  }
    };
    g.FillPolygon(&body, pts, 4);

    // 亮紫核心
    Gdiplus::SolidBrush core(Gdiplus::Color(255, 140, 255));
    g.FillEllipse(&core, m_x - 7.0f, m_y - 7.0f, 14.0f, 14.0f);
}
