#pragma once

#include "entity/PowerUp.h"
#include "entity/Player.h"
#include <vector>
#include <memory>
#include <random>

// ============================================================
// 道具系统 — 管理道具生成和效果
// ============================================================
class PowerUpSystem {
public:
    PowerUpSystem();

    // 尝试掉落道具（由敌机死亡触发）
    void TryDrop(float x, float y, bool isBoss,
                 std::vector<std::unique_ptr<PowerUp>>& powerUps);

    // Boss 击败时的道具雨
    void SpawnBossDrops(float x, float y,
                        std::vector<std::unique_ptr<PowerUp>>& powerUps);

    // 应用道具效果
    void Apply(Player& player, PowerUpType type);

private:
    std::mt19937 m_rng;
};
