#include "entity/Bullet.h"

void Bullet::Init(float x, float y, float vx, float vy,
                  int damage, BulletOwner owner, bool isLaser) {
    m_x       = x;
    m_y       = y;
    m_vx      = vx;
    m_vy      = vy;
    m_damage  = damage;
    m_owner   = owner;
    m_isLaser = isLaser;
    m_laserLife = 0.08f;  // 激光每帧刷新
    m_active  = true;

    if (isLaser) {
        SetSize(static_cast<float>(Config::LASER_WIDTH), 60.0f);
        SetHitbox(static_cast<float>(Config::LASER_WIDTH), 60.0f);
    } else {
        SetSize(static_cast<float>(Config::BULLET_WIDTH),
                static_cast<float>(Config::BULLET_HEIGHT));
        SetHitbox(static_cast<float>(Config::BULLET_WIDTH),
                  static_cast<float>(Config::BULLET_HEIGHT));
    }
}

void Bullet::Update(float dt) {
    if (!m_active) return;

    Entity::Update(dt);

    // 激光时间衰减
    if (m_isLaser) {
        m_laserLife -= dt;
        if (m_laserLife <= 0.0f) {
            Deactivate();
        }
    }

    // 超出屏幕销毁
    if (m_y < -50.0f || m_y > Config::CANVAS_HEIGHT + 50.0f
        || m_x < -50.0f || m_x > Config::CANVAS_WIDTH + 50.0f) {
        Deactivate();
    }
}

void Bullet::Render(Gdiplus::Graphics& g) {
    if (!m_active) return;

    if (m_isLaser) {
        // 激光：竖直矩形 + 发光效果
        Gdiplus::Color coreColor;
        Gdiplus::Color glowColor;

        if (m_owner == BulletOwner::PLAYER) {
            coreColor = Gdiplus::Color(220, 255, 255, 200);
            glowColor = Gdiplus::Color(120, 100, 200, 255);
        } else {
            coreColor = Gdiplus::Color(220, 255, 100, 80);
            glowColor  = Gdiplus::Color(120, 255, 50,  30);
        }

        // 外发光
        Gdiplus::SolidBrush glowBrush(glowColor);
        g.FillRectangle(&glowBrush, m_x - 8.0f, m_y - 50.0f, 16.0f, 100.0f);

        // 内核
        Gdiplus::SolidBrush coreBrush(coreColor);
        g.FillRectangle(&coreBrush, m_x - 3.0f, m_y - 48.0f, 6.0f, 96.0f);

    } else {
        // 普通子弹：小椭圆
        Gdiplus::Color color;
        if (m_owner == BulletOwner::PLAYER) {
            color = Gdiplus::Color(255, 255, 100);
        } else {
            color = Gdiplus::Color(255, 255, 60, 60);
        }

        Gdiplus::SolidBrush brush(color);
        g.FillEllipse(&brush, m_x - m_width * 0.5f, m_y - m_height * 0.5f,
                      m_width, m_height);

        // 发光外圈
        Gdiplus::Color glow(100, color.GetR(), color.GetG(), color.GetB());
        Gdiplus::SolidBrush glowBrush(glow);
        g.FillEllipse(&glowBrush, m_x - m_width * 0.8f, m_y - m_height * 0.8f,
                      m_width * 1.6f, m_height * 1.6f);
    }
}
