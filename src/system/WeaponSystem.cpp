#include "system/WeaponSystem.h"
#include <cmath>

void WeaponSystem::Fire(Player& player,
                        ObjectPool<Bullet, Config::MAX_BULLETS>& bulletPool) {
    int level = player.GetWeaponLevel();
    switch (level) {
    case 0: FireSingle(player, bulletPool); break;
    case 1: FireDouble(player, bulletPool); break;
    case 2: FireTriple(player, bulletPool); break;
    case 3: FireSpread(player, bulletPool); break;
    case 4: FireLaser(player, bulletPool);  break;
    default: FireSingle(player, bulletPool); break;
    }
}

void WeaponSystem::FireSingle(Player& player,
                              ObjectPool<Bullet, Config::MAX_BULLETS>& pool) {
    Bullet* b = pool.Acquire();
    if (!b) return;
    b->Init(player.GetFirePosX(), player.GetFirePosY(),
            0.0f, -Config::BULLET_SPEED,
            1, BulletOwner::PLAYER);
}

void WeaponSystem::FireDouble(Player& player,
                              ObjectPool<Bullet, Config::MAX_BULLETS>& pool) {
    float x = player.GetFirePosX();
    float y = player.GetFirePosY();

    Bullet* b1 = pool.Acquire();
    if (b1) b1->Init(x - 6.0f, y, 0.0f, -Config::BULLET_SPEED,
                     1, BulletOwner::PLAYER);

    Bullet* b2 = pool.Acquire();
    if (b2) b2->Init(x + 6.0f, y, 0.0f, -Config::BULLET_SPEED,
                     1, BulletOwner::PLAYER);
}

void WeaponSystem::FireTriple(Player& player,
                              ObjectPool<Bullet, Config::MAX_BULLETS>& pool) {
    float x = player.GetFirePosX();
    float y = player.GetFirePosY();

    Bullet* b1 = pool.Acquire();
    if (b1) b1->Init(x, y, 0.0f, -Config::BULLET_SPEED,
                     1, BulletOwner::PLAYER);

    float angleSpeed = Config::BULLET_SPEED * 0.85f;
    float offset = 2.5f;

    Bullet* b2 = pool.Acquire();
    if (b2) b2->Init(x - 8.0f, y, -offset, -angleSpeed,
                     1, BulletOwner::PLAYER);

    Bullet* b3 = pool.Acquire();
    if (b3) b3->Init(x + 8.0f, y, offset, -angleSpeed,
                     1, BulletOwner::PLAYER);
}

void WeaponSystem::FireSpread(Player& player,
                              ObjectPool<Bullet, Config::MAX_BULLETS>& pool) {
    float x = player.GetFirePosX();
    float y = player.GetFirePosY();

    // 5路扇形弹幕
    float baseSpeed = Config::BULLET_SPEED;
    float angles[] = { -0.25f, -0.12f, 0.0f, 0.12f, 0.25f };  // 弧度

    for (int i = 0; i < 5; ++i) {
        Bullet* b = pool.Acquire();
        if (!b) break;
        float vx = sin(angles[i]) * baseSpeed;
        float vy = -cos(angles[i]) * baseSpeed;
        b->Init(x, y, vx, vy, 1, BulletOwner::PLAYER);
    }
}

void WeaponSystem::FireLaser(Player& player,
                             ObjectPool<Bullet, Config::MAX_BULLETS>& pool) {
    float x = player.GetFirePosX();
    float y = player.GetFirePosY() - 30.0f;  // 激光从飞机上方开始

    // 持续型激光：生成一个长激光子弹，每帧刷新
    Bullet* laser = pool.Acquire();
    if (laser) {
        laser->Init(x, y, 0.0f, 0.0f, 3, BulletOwner::PLAYER, true);
    }
}
