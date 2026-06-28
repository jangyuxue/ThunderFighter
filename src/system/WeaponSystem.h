#pragma once

#include "entity/Player.h"
#include "entity/Bullet.h"
#include "core/ObjectPool.h"
#include "config/GameConfig.h"

// ============================================================
// 武器系统 — 管理射击模式和子弹生成
// ============================================================
class WeaponSystem {
public:
    WeaponSystem() = default;

    void Fire(Player& player,
              ObjectPool<Bullet, Config::MAX_BULLETS>& bulletPool);

private:
    void FireSingle(Player& player,
                    ObjectPool<Bullet, Config::MAX_BULLETS>& pool);
    void FireDouble(Player& player,
                    ObjectPool<Bullet, Config::MAX_BULLETS>& pool);
    void FireTriple(Player& player,
                    ObjectPool<Bullet, Config::MAX_BULLETS>& pool);
    void FireSpread(Player& player,
                    ObjectPool<Bullet, Config::MAX_BULLETS>& pool);
    void FireLaser(Player& player,
                   ObjectPool<Bullet, Config::MAX_BULLETS>& pool);

    float GetShootInterval(int weaponLevel) const;
};
