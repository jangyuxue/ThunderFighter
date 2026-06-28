#pragma once

#include "config/GameConfig.h"
#include <string>
#include <vector>

// ============================================================
// 关卡/波次数据定义（数据驱动）
// ============================================================
enum class EnemySpawnType {
    SMALL,
    MEDIUM,
    MIX       // 混合
};

struct WaveDef {
    EnemySpawnType enemyType;
    int   count;
    float spawnInterval;  // 生成间隔（秒）
    float delayBefore;    // 开始前等待（秒）
};

struct LevelDef {
    int   levelNumber;
    std::string levelName;
    std::vector<WaveDef> waves;
    bool  hasBoss;
    int   bossHP;
    float enemySpeedMul;      // 敌人速度倍率
    float enemyShootMul;      // 敌人射击频率倍率
};

// 预定义的 5 关数据
inline const std::vector<LevelDef> LEVELS = {
    // 第1关：地球轨道（教学关）
    { 1, "Earth Orbit", {
        { EnemySpawnType::SMALL,  6,  1.0f, 1.5f },
        { EnemySpawnType::SMALL,  8,  0.8f, 3.0f },
        { EnemySpawnType::SMALL, 10,  0.6f, 3.0f },
    }, true, 60, 0.8f, 0.5f },

    // 第2关：小行星带
    { 2, "Asteroid Belt", {
        { EnemySpawnType::SMALL,  8,  0.8f, 1.5f },
        { EnemySpawnType::MEDIUM, 5,  1.2f, 3.0f },
        { EnemySpawnType::MIX,   10,  0.6f, 3.0f },
        { EnemySpawnType::MEDIUM, 6,  1.0f, 3.0f },
    }, true, 100, 1.0f, 0.8f },

    // 第3关：敌方舰队
    { 3, "Enemy Fleet", {
        { EnemySpawnType::MEDIUM, 6,  1.0f, 1.5f },
        { EnemySpawnType::MIX,   12,  0.5f, 2.5f },
        { EnemySpawnType::MEDIUM, 8,  0.8f, 3.0f },
        { EnemySpawnType::MIX,   15,  0.4f, 3.0f },
    }, true, 150, 1.2f, 1.0f },

    // 第4关：深空要塞
    { 4, "Deep Space Fortress", {
        { EnemySpawnType::MEDIUM, 8,  0.8f, 1.5f },
        { EnemySpawnType::MIX,   15,  0.4f, 2.5f },
        { EnemySpawnType::MEDIUM,10,  0.6f, 3.0f },
        { EnemySpawnType::MIX,   20,  0.3f, 2.5f },
        { EnemySpawnType::MEDIUM, 8,  0.7f, 3.0f },
    }, true, 200, 1.3f, 1.3f },

    // 第5关：母舰决战
    { 5, "Mothership", {
        { EnemySpawnType::MEDIUM, 8,  0.8f, 1.5f },
        { EnemySpawnType::MIX,   12,  0.5f, 2.5f },
        { EnemySpawnType::MEDIUM,10,  0.6f, 3.0f },
        { EnemySpawnType::MIX,   18,  0.3f, 2.5f },
        { EnemySpawnType::MEDIUM,12,  0.5f, 3.0f },
        { EnemySpawnType::MIX,   20,  0.3f, 2.5f },
    }, true, 300, 1.5f, 1.5f },
};
