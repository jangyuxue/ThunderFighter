#include "system/LevelManager.h"
#include <random>

LevelManager::LevelManager() = default;

void LevelManager::Reset() {
    m_currentLevel   = 1;
    m_currentWave    = 0;
    m_spawnedInWave  = 0;
    m_waveTimer      = 0.0f;
    m_spawnTimer     = 0.0f;
    m_waveDelayTimer = 0.0f;
    m_waveActive     = false;
    m_allWavesDone   = false;
    m_bossActive     = false;
    m_bossSpawned    = false;
    m_bossDefeated   = false;
    m_levelComplete  = false;
    m_allComplete    = false;
    m_inTransition   = false;
    m_transitionTimer = 0.0f;
    m_currentDef     = nullptr;
}

void LevelManager::StartLevel(int level) {
    m_currentLevel  = level;
    m_currentWave   = 0;
    m_spawnedInWave = 0;
    m_waveTimer     = 0.0f;
    m_spawnTimer    = 0.0f;
    m_waveDelayTimer = 0.0f;
    m_waveActive    = false;
    m_allWavesDone  = false;
    m_bossActive    = false;
    m_bossSpawned   = false;
    m_bossDefeated  = false;
    m_levelComplete = false;
    m_inTransition  = false;

    if (level <= static_cast<int>(LEVELS.size())) {
        m_currentDef = &LEVELS[level - 1];
    } else {
        m_allComplete = true;
        m_currentDef = nullptr;
    }
}

void LevelManager::Update(float dt,
                          std::vector<std::unique_ptr<Enemy>>& enemies,
                          std::vector<std::unique_ptr<PowerUp>>& /*powerUps*/) {
    // 先处理过渡计时器（即使 m_levelComplete 也要倒计时）
    if (m_inTransition) {
        m_transitionTimer -= dt;
        if (m_transitionTimer <= 0.0f) {
            m_inTransition = false;
            m_levelComplete = false;
            if (m_allComplete) return;
        }
        return;
    }

    if (m_levelComplete || m_allComplete) return;
    if (!m_currentDef) return;

    // Boss 阶段
    if (m_bossActive) {
        // 检查 Boss 是否被击败
        return;
    }

    if (m_allWavesDone && !m_bossSpawned && m_currentDef->hasBoss) {
        SpawnBoss(m_currentLevel, enemies);
        m_bossSpawned = true;
        m_bossActive  = true;
        return;
    }

    if (m_allWavesDone && !m_currentDef->hasBoss) {
        m_levelComplete = true;
        return;
    }

    // 波次管理
    if (m_currentWave >= static_cast<int>(m_currentDef->waves.size())) {
        m_allWavesDone = true;
        return;
    }

    const WaveDef& wave = m_currentDef->waves[m_currentWave];

    if (!m_waveActive) {
        m_waveDelayTimer += dt;
        if (m_waveDelayTimer >= wave.delayBefore) {
            m_waveActive = true;
            m_spawnTimer = wave.spawnInterval;  // 立即生成第一个
        }
        return;
    }

    m_spawnTimer += dt;
    while (m_spawnTimer >= wave.spawnInterval && m_spawnedInWave < wave.count) {
        m_spawnTimer -= wave.spawnInterval;

        float spawnX = 40.0f + static_cast<float>(rand() % (Config::CANVAS_WIDTH - 80));
        float spawnY = -30.0f - static_cast<float>(rand() % 40);

        switch (wave.enemyType) {
        case EnemySpawnType::SMALL:
            SpawnSmall(spawnX, spawnY, m_currentLevel, enemies);
            break;
        case EnemySpawnType::MEDIUM:
            SpawnMedium(spawnX, spawnY, m_currentLevel, enemies);
            break;
        case EnemySpawnType::MIX:
            SpawnMixed(spawnX, spawnY, m_currentLevel, enemies);
            break;
        }

        ++m_spawnedInWave;
    }

    // 当前波次完成
    if (m_spawnedInWave >= wave.count) {
        ++m_currentWave;
        m_spawnedInWave  = 0;
        m_spawnTimer     = 0.0f;
        m_waveDelayTimer = 0.0f;
        m_waveActive     = false;
    }
}

void LevelManager::SpawnSmall(float x, float y, int level,
                              std::vector<std::unique_ptr<Enemy>>& enemies) {
    auto e = std::make_unique<EnemySmall>();
    e->Init(x, y, level);
    enemies.push_back(std::move(e));
}

void LevelManager::SpawnMedium(float x, float y, int level,
                               std::vector<std::unique_ptr<Enemy>>& enemies) {
    auto e = std::make_unique<EnemyMedium>();
    e->Init(x, y, level);
    enemies.push_back(std::move(e));
}

void LevelManager::SpawnBoss(int level,
                             std::vector<std::unique_ptr<Enemy>>& enemies) {
    auto boss = std::make_unique<EnemyBoss>();
    boss->Init(Config::CANVAS_WIDTH * 0.5f, -60.0f, level);
    enemies.push_back(std::move(boss));
}

void LevelManager::SpawnMixed(float x, float y, int level,
                              std::vector<std::unique_ptr<Enemy>>& enemies) {
    if (rand() % 2 == 0) {
        SpawnSmall(x, y, level, enemies);
    } else {
        SpawnMedium(x, y, level, enemies);
    }
}

void LevelManager::OnBossDefeated() {
    m_bossDefeated = true;
    m_bossActive   = false;
    m_levelComplete = true;

    if (m_currentLevel >= Config::MAX_LEVELS) {
        m_allComplete = true;
    } else {
        // 关卡过渡
        m_inTransition = true;
        m_transitionTimer = Config::LEVEL_TRANSITION_TIME;
        ++m_currentLevel;
    }
}

const std::string& LevelManager::GetLevelName() const {
    static const std::string none = "Unknown";
    return m_currentDef ? m_currentDef->levelName : none;
}
