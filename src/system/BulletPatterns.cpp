#include "system/BulletPatterns.h"
#include <cmath>

namespace BulletPattern {

void AimedSpread(EnemyBulletPool& pool, float sx, float sy,
                 float tx, float ty, int count, float spreadAngle, float speed) {
    float baseAngle = atan2(tx - sx, ty - sy);
    int half = count / 2;
    for (int i = 0; i < count; ++i) {
        float offset = (count % 2 == 0)
            ? (i - half + 0.5f) * spreadAngle
            : (i - half) * spreadAngle;
        float a = baseAngle + offset;
        Bullet* b = pool.Acquire();
        if (b) {
            b->Init(sx, sy, sin(a) * speed, cos(a) * speed,
                    1, BulletOwner::ENEMY);
        }
    }
}

void RadialFan(EnemyBulletPool& pool, float sx, float sy,
               int count, float speed) {
    for (int i = 0; i < count; ++i) {
        float a = i * 3.14159265f * 2.0f / count;
        Bullet* b = pool.Acquire();
        if (b) {
            b->Init(sx, sy, sin(a) * speed, cos(a) * speed,
                    1, BulletOwner::ENEMY);
        }
    }
}

void Spiral(EnemyBulletPool& pool, float sx, float sy,
            int count, float speed, float& angle, float angleStep) {
    angle += angleStep;
    for (int i = 0; i < count; ++i) {
        float a = angle + i * 3.14159265f * 2.0f / count;
        Bullet* b = pool.Acquire();
        if (b) {
            b->Init(sx, sy, sin(a) * speed, cos(a) * speed,
                    1, BulletOwner::ENEMY);
        }
    }
}

void WaveWall(EnemyBulletPool& pool, float sx, float sy,
              int count, float speed) {
    // 横向均匀分布，整体略向下
    float totalW = (count - 1) * 24.0f;
    float startX = sx - totalW * 0.5f;
    for (int i = 0; i < count; ++i) {
        float bx = startX + i * 24.0f;
        float dx = (bx - sx) * 0.02f;  // 轻微横向发散
        Bullet* b = pool.Acquire();
        if (b) {
            b->Init(bx, sy, dx, speed, 1, BulletOwner::ENEMY);
        }
    }
}

void CrossBurst(EnemyBulletPool& pool, float sx, float sy, float speed) {
    // 下、左、右、左下、右下 五个方向
    const float dirs[5][2] = {
        { 0.0f,       1.0f      },  // 下
        { -1.0f,      0.0f      },  // 左
        { 1.0f,       0.0f      },  // 右
        { -0.7071f,   0.7071f   },  // 左下
        { 0.7071f,    0.7071f   }   // 右下
    };
    for (int i = 0; i < 5; ++i) {
        Bullet* b = pool.Acquire();
        if (b) {
            b->Init(sx, sy, dirs[i][0] * speed, dirs[i][1] * speed,
                    1, BulletOwner::ENEMY);
        }
    }
}

} // namespace BulletPattern
