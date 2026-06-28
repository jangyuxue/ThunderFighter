#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include "core/Renderable.h"
#include "config/GameConfig.h"
#include <cmath>

// ============================================================
// 实体基类 — 所有游戏对象的基础
// ============================================================
class Entity : public Renderable {
public:
    Entity() = default;
    virtual ~Entity() = default;

    // ---- 属性 ----
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    void  SetX(float x) { m_x = x; }
    void  SetY(float y) { m_y = y; }
    void  SetPosition(float x, float y) { m_x = x; m_y = y; }

    float GetVelocityX() const { return m_vx; }
    float GetVelocityY() const { return m_vy; }
    void  SetVelocityX(float vx) { m_vx = vx; }
    void  SetVelocityY(float vy) { m_vy = vy; }
    void  SetVelocity(float vx, float vy) { m_vx = vx; m_vy = vy; }

    float GetWidth()  const { return m_width; }
    float GetHeight() const { return m_height; }
    void  SetSize(float w, float h) { m_width = w; m_height = h; }

    bool IsActive() const { return m_active; }
    void SetActive(bool active) { m_active = active; }
    void Activate()   { m_active = true; }
    void Deactivate() { m_active = false; }

    // ---- 碰撞盒（AABB） ----
    float Left()   const { return m_x - m_hitboxW * 0.5f; }
    float Right()  const { return m_x + m_hitboxW * 0.5f; }
    float Top()    const { return m_y - m_hitboxH * 0.5f; }
    float Bottom() const { return m_y + m_hitboxH * 0.5f; }
    void  SetHitbox(float w, float h) { m_hitboxW = w; m_hitboxH = h; }

    // AABB 碰撞检测
    bool CollidesWith(const Entity& other) const {
        return std::abs(m_x - other.m_x) < (m_hitboxW + other.m_hitboxW) * 0.5f
            && std::abs(m_y - other.m_y) < (m_hitboxH + other.m_hitboxH) * 0.5f;
    }

    // 是否在屏幕内（含边距）
    bool IsOnScreen(float margin = 50.0f) const {
        return m_y > -margin
            && m_y < 720.0f + margin
            && m_x > -margin
            && m_x < 480.0f + margin;
    }

    // ---- 更新 ----
    virtual void Update(float dt) {
        m_x += m_vx * dt * 60.0f;
        m_y += m_vy * dt * 60.0f;
    }

protected:
    float m_x = 0.0f, m_y = 0.0f;
    float m_vx = 0.0f, m_vy = 0.0f;
    float m_width = 20.0f, m_height = 20.0f;
    float m_hitboxW = 16.0f, m_hitboxH = 16.0f;
    bool  m_active = false;
};
