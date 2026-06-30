#pragma once

#include "core/ObjectPool.h"
#include "entity/Bullet.h"
#include "config/GameConfig.h"

// ============================================================
// 敌方弹幕模式库
// 将 Game.cpp 中硬编码的弹幕生成抽离为可复用函数
// ============================================================
namespace BulletPattern {

using EnemyBulletPool = ObjectPool<Bullet, Config::MAX_BULLETS>;

// N 向瞄准扇形散射：以 (sx,sy) 为起点，向目标点 (tx,ty) 方向呈扇形发射 count 发
void AimedSpread(EnemyBulletPool& pool, float sx, float sy,
                 float tx, float ty, int count, float spreadAngle, float speed);

// 均匀圆周放射：count 发子弹均匀分布
void RadialFan(EnemyBulletPool& pool, float sx, float sy,
               int count, float speed);

// 旋转螺旋：每次调用 angle 增加 angleStep，发射 count 发
void Spiral(EnemyBulletPool& pool, float sx, float sy,
            int count, float speed, float& angle, float angleStep);

// 波浪墙：水平一排 count 发子弹，略带横向发散
void WaveWall(EnemyBulletPool& pool, float sx, float sy,
              int count, float speed);

// 十字/斜角散射：5 向（下、左下、右下、左、右）
void CrossBurst(EnemyBulletPool& pool, float sx, float sy, float speed);

} // namespace BulletPattern
