#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include <vector>
#include <memory>

#include "core/Window.h"
#include "core/GameTimer.h"
#include "core/InputManager.h"
#include "core/Renderer.h"
#include "core/ObjectPool.h"
#include "entity/Entity.h"
#include "entity/Player.h"
#include "entity/Bullet.h"
#include "entity/Enemy.h"
#include "entity/PowerUp.h"
#include "entity/Particle.h"
#include "system/StarField.h"
#include "system/LevelManager.h"
#include "system/WeaponSystem.h"
#include "system/PowerUpSystem.h"
#include "system/ParticleSystem.h"
#include "system/ScoreManager.h"
#include "ui/UIManager.h"
#include "config/GameConfig.h"
#include "config/LevelData.h"

// ============================================================
// 游戏核心 — 拥有所有管理器，运行主循环
// ============================================================
class Game {
public:
    Game();
    ~Game();

    bool Initialize(HWND hWnd);
    void Run();
    void Shutdown();

private:
    void FixedUpdate(double dt);
    void Render(double interpolationAlpha);

    // 消息处理
    void ProcessMessages();

    // 游戏状态
    void StartNewGame();
    void UpdatePlayer(float dt);
    void UpdateEnemies(float dt);
    void UpdateBullets(float dt);
    void UpdatePowerUps(float dt);
    void ProcessCollisions();
    void CleanupDeadEntities();
    void CheckLevelTransition();

    // ---- 核心引擎 ----
    HWND          m_hWnd = nullptr;
    GameTimer     m_timer;
    InputManager  m_input;
    Renderer      m_renderer;
    bool          m_isRunning = false;

    // ---- 游戏对象 ----
    Player       m_player;

    // 对象池 — 高性能
    ObjectPool<Bullet, Config::MAX_BULLETS>    m_playerBullets;
    ObjectPool<Bullet, Config::MAX_BULLETS>    m_enemyBullets;
    ObjectPool<Particle, Config::MAX_PARTICLES> m_particlePool;

    // 动态分配 — 数量较少
    std::vector<std::unique_ptr<Enemy>>    m_enemies;
    std::vector<std::unique_ptr<PowerUp>>  m_powerUps;

    // ---- 系统 ----
    StarField       m_starField;
    LevelManager    m_levelManager;
    WeaponSystem    m_weaponSystem;
    PowerUpSystem   m_powerUpSystem;
    ParticleSystem  m_particleSystem;
    ScoreManager    m_scoreManager;
    UIManager       m_uiManager;
};
