#pragma once

#include "config/LevelData.h"
#include "entity/Enemy.h"
#include "entity/PowerUp.h"
#include <vector>
#include <memory>
#include <functional>

// ============================================================
// 关卡管理器 — 控制波次生成和关卡进度
// ============================================================
class LevelManager {
public:
    LevelManager();

    void Reset();
    void StartLevel(int level);

    // 每帧更新
    void Update(float dt,
                std::vector<std::unique_ptr<Enemy>>& enemies,
                std::vector<std::unique_ptr<PowerUp>>& powerUps);

    // 状态查询
    int  GetCurrentLevel() const   { return m_currentLevel; }
    bool IsLevelComplete() const   { return m_levelComplete; }
    bool IsBossActive()    const   { return m_bossActive; }
    bool IsBossDefeated()  const   { return m_bossDefeated; }
    bool IsAllLevelsComplete() const { return m_allComplete; }
    bool IsInTransition()  const   { return m_inTransition; }
    float GetTransitionTimer() const { return m_transitionTimer; }

    // 通知 Boss 被击败
    void OnBossDefeated();

    // 获取当前关卡名
    const std::string& GetLevelName() const;

private:
    void SpawnSmall(float x, float y, int level,
                    std::vector<std::unique_ptr<Enemy>>& enemies);
    void SpawnMedium(float x, float y, int level,
                     std::vector<std::unique_ptr<Enemy>>& enemies);
    void SpawnBoss(int level,
                   std::vector<std::unique_ptr<Enemy>>& enemies);
    void SpawnMixed(float x, float y, int level,
                    std::vector<std::unique_ptr<Enemy>>& enemies);

    int    m_currentLevel   = 1;
    int    m_currentWave    = 0;
    int    m_spawnedInWave  = 0;
    float  m_waveTimer      = 0.0f;
    float  m_spawnTimer     = 0.0f;
    float  m_waveDelayTimer = 0.0f;
    bool   m_waveActive     = false;
    bool   m_allWavesDone   = false;

    bool   m_bossActive     = false;
    bool   m_bossSpawned    = false;
    bool   m_bossDefeated   = false;
    bool   m_levelComplete  = false;
    bool   m_allComplete    = false;

    bool   m_inTransition    = false;
    float  m_transitionTimer = 0.0f;

    const LevelDef* m_currentDef = nullptr;
};
