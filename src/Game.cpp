#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

// ============================================================
// 构造/析构
// ============================================================
Game::Game() {
    srand(static_cast<unsigned>(time(nullptr)));
}

Game::~Game() {
    Shutdown();
}

// ============================================================
// 初始化
// ============================================================
bool Game::Initialize(HWND hWnd) {
    m_hWnd = hWnd;

    if (!m_renderer.Initialize(hWnd, Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT)) {
        return false;
    }

    m_input.SetWindow(hWnd);
    m_uiManager.Init();      // 字体创建需在 GDI+ 启动后
    m_particleSystem.SetPool(&m_particlePool);
    m_starField.Init();
    m_isRunning = true;

    return true;
}

// ============================================================
// 主循环
// ============================================================
void Game::Run() {
    m_timer.Reset();

    double accumulator = 0.0;
    const double dt = Config::FIXED_TIMESTEP;

    while (m_isRunning) {
        ProcessMessages();

        double frameTime = m_timer.Tick();
        accumulator += frameTime;

        int steps = 0;
        while (accumulator >= dt && steps < Config::MAX_FRAMESKIP) {
            FixedUpdate(dt);
            accumulator -= dt;
            ++steps;
        }

        Render(accumulator / dt);
    }
}

void Game::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            m_isRunning = false;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// ============================================================
// 固定步长更新
// ============================================================
void Game::FixedUpdate(double dt) {
    m_input.Update();
    m_uiManager.Update(static_cast<float>(dt));

    GameState state = m_uiManager.GetState();

    if (state == GameState::HUB || state == GameState::HIGH_SCORE
        || state == GameState::LEVEL_SELECT || state == GameState::SHOP
        || state == GameState::MISSIONS || state == GameState::ACHIEVEMENTS) {
        m_uiManager.HandleInput(m_input, m_player, m_scoreManager);

        // HUB/关卡选择 → 开始游戏
        if (m_uiManager.ShouldStartGame()) {
            m_uiManager.ResetStartFlag();
            m_uiManager.SetState(GameState::PLAYING);
            m_levelManager.Reset();
            m_scoreManager.ResetRun();
            m_enemies.clear();
            m_powerUps.clear();
            m_playerBullets.ReleaseAll();
            m_enemyBullets.ReleaseAll();
            m_particlePool.ReleaseAll();
            m_player.Init(static_cast<PlayerType>(m_uiManager.GetSelectedAircraft()));
        }

        m_starField.Update(static_cast<float>(dt));
        return;
    }

    if (state == GameState::GAME_OVER) {
        m_uiManager.HandleInput(m_input, m_player, m_scoreManager);
        m_particleSystem.Update(static_cast<float>(dt));
        return;
    }

    if (state == GameState::PLAYING || state == GameState::LEVEL_TRANSITION) {
        m_uiManager.HandleInput(m_input, m_player, m_scoreManager);

        // 关卡过渡中：仍需要调用 Update 来倒计时
        if (state == GameState::LEVEL_TRANSITION) {
            m_levelManager.Update(static_cast<float>(dt), m_enemies, m_powerUps);
            if (!m_levelManager.IsInTransition()) {
                m_levelManager.StartLevel(m_levelManager.GetCurrentLevel());
                m_uiManager.SetState(GameState::PLAYING);
                m_weaponSystem = WeaponSystem();
            }
            m_starField.Update(static_cast<float>(dt));
            m_particleSystem.Update(static_cast<float>(dt));
            return;
        }

        // === 正常游戏逻辑 ===

        // 首次进入游戏，启动所选关卡
        if (!m_levelManager.IsStarted()) {
            m_levelManager.StartLevel(m_uiManager.GetSelectedLevel());
            m_scoreManager.ResetRun();
        }

        m_starField.Update(static_cast<float>(dt));

        // 检查玩家死亡（放在更新之前，防止死亡后仍能操作）
        if (!m_player.IsAlive() || !m_player.IsActive()) {
            m_particleSystem.EmitBossExplosion(m_player.GetX(), m_player.GetY());
            CleanupDeadEntities();
            m_uiManager.SetState(GameState::GAME_OVER);
            return;
        }

        // 玩家更新
        UpdatePlayer(static_cast<float>(dt));

        // 关卡管理（生成敌人）
        m_levelManager.Update(static_cast<float>(dt), m_enemies, m_powerUps);

        // 敌人更新和射击
        UpdateEnemies(static_cast<float>(dt));

        // 子弹更新
        UpdateBullets(static_cast<float>(dt));

        // 道具更新
        UpdatePowerUps(static_cast<float>(dt));

        // 粒子更新
        m_particleSystem.Update(static_cast<float>(dt));

        // 碰撞检测
        ProcessCollisions();

        // 清理死亡实体
        CleanupDeadEntities();

        // 关卡完成检查
        CheckLevelTransition();

        // Boss 检查
        if (m_levelManager.IsBossActive()) {
            // 检查 Boss 是否还活着
            bool bossAlive = false;
            for (auto& e : m_enemies) {
                if (e->GetKind() == EnemyKind::BOSS && e->IsActive() && !e->IsDead()) {
                    bossAlive = true;
                    break;
                }
            }
            if (!bossAlive) {
                m_levelManager.OnBossDefeated();
                // 分数已在 ProcessCollisions 中添加，不重复计算
                m_powerUpSystem.SpawnBossDrops(
                    Config::CANVAS_WIDTH * 0.5f, 120.0f, m_powerUps);

                // Boss 爆炸特效
                m_particleSystem.EmitBossExplosion(
                    Config::CANVAS_WIDTH * 0.5f, 120.0f);
            }
        }
    }

    if (state == GameState::PAUSED) {
        m_uiManager.HandleInput(m_input, m_player, m_scoreManager);
    }
}

// ============================================================
// 玩家更新
// ============================================================
void Game::UpdatePlayer(float dt) {
    float dx = m_input.GetHorizontal();
    float dy = m_input.GetVertical();
    bool shooting = m_input.IsShooting();

    m_player.Update(dt, dx, dy, shooting);

    // 射击逻辑
    if (shooting && m_player.GetShootCooldown() <= 0.0f) {
        if (m_player.GetWeaponLevel() >= 4) {
            // 激光模式：每帧都发射激光
            m_weaponSystem.Fire(m_player, m_playerBullets);
            m_player.GetShootCooldown() = m_player.GetShootInterval();
        } else {
            m_weaponSystem.Fire(m_player, m_playerBullets);
            m_player.GetShootCooldown() = m_player.GetShootInterval();
        }
    }

    // 炸弹
    if (m_input.IsBombPressed() && m_player.GetBombs() > 0) {
        m_player.UseBomb();  // 消耗一个炸弹
        // 清屏
        for (auto& e : m_enemies) {
            if (e->IsActive() && !e->IsDying()) {
                e->TakeDamage(999);
                if (e->IsDead() || e->IsDying()) {
                    m_scoreManager.AddScore(e->GetScoreValue());
                    m_particleSystem.EmitExplosion(e->GetX(), e->GetY(), 30.0f, 20);
                }
            }
        }
        // 清除所有敌方子弹
        for (auto& b : m_enemyBullets) {
            if (b.IsActive()) b.Deactivate();
        }
        // 屏幕闪白效果
        m_particleSystem.EmitExplosion(
            Config::CANVAS_WIDTH * 0.5f,
            Config::CANVAS_HEIGHT * 0.5f,
            200.0f, 60);
    }

    // 引擎拖尾
    m_particleSystem.EmitEngineTrail(
        m_player.GetX() - 5.0f,
        m_player.GetY() + 18.0f);
    m_particleSystem.EmitEngineTrail(
        m_player.GetX() + 5.0f,
        m_player.GetY() + 18.0f);
}

// ============================================================
// 敌人更新
// ============================================================
void Game::UpdateEnemies(float dt) {
    for (auto& enemy : m_enemies) {
        if (!enemy->IsActive()) continue;

        enemy->Update(dt);

        // 中型和 Boss 敌机射击
        if (enemy->ShouldShoot(dt)) {
            if (enemy->GetKind() == EnemyKind::BOSS) {
                // Boss 弹幕
                EnemyBoss* boss = static_cast<EnemyBoss*>(enemy.get());
                int pattern = boss->GetPhase() > 1 ? rand() % 3 : 0;
                float sx = boss->GetShootX();
                float sy = boss->GetShootY();

                switch (pattern) {
                case 0: {
                    // 瞄准射击
                    float angle = atan2(m_player.GetX() - sx, -100);
                    for (int i = -1; i <= 1; ++i) {
                        Bullet* b = m_enemyBullets.Acquire();
                        if (b) {
                            b->Init(sx + i * 10.0f, sy,
                                    sin(angle + i * 0.1f) * Config::ENEMY_BULLET_SPEED * 0.5f,
                                    Config::ENEMY_BULLET_SPEED,
                                    1, BulletOwner::ENEMY);
                        }
                    }
                    break;
                }
                case 1: {
                    // 扇形弹幕
                    for (int i = 0; i < 8; ++i) {
                        float a = i * 3.14159f * 2.0f / 8.0f;
                        Bullet* b = m_enemyBullets.Acquire();
                        if (b) {
                            b->Init(sx, sy,
                                    sin(a) * Config::ENEMY_BULLET_SPEED * 0.7f,
                                    cos(a) * Config::ENEMY_BULLET_SPEED * 0.7f,
                                    1, BulletOwner::ENEMY);
                        }
                    }
                    break;
                }
                case 2: {
                    // 螺旋弹幕
                    static float spiralAngle = 0.0f;
                    spiralAngle += 0.3f;
                    for (int i = 0; i < 3; ++i) {
                        float a = spiralAngle + i * 3.14159f * 2.0f / 3.0f;
                        Bullet* b = m_enemyBullets.Acquire();
                        if (b) {
                            b->Init(sx, sy,
                                    sin(a) * Config::ENEMY_BULLET_SPEED * 0.5f,
                                    Config::ENEMY_BULLET_SPEED * 0.6f,
                                    1, BulletOwner::ENEMY);
                        }
                    }
                    break;
                }
                }
            } else if (enemy->GetKind() == EnemyKind::MEDIUM) {
                // 中型敌机：瞄准玩家射击
                Bullet* b = m_enemyBullets.Acquire();
                if (b) {
                    float dx = m_player.GetX() - enemy->GetShootX();
                    float dy2 = 100.0f;  // 向下
                    float len = sqrt(dx * dx + dy2 * dy2);
                    float vx = dx / len * Config::ENEMY_BULLET_SPEED;
                    float vy = dy2 / len * Config::ENEMY_BULLET_SPEED;
                    b->Init(enemy->GetShootX(), enemy->GetShootY(),
                            vx, vy, 1, BulletOwner::ENEMY);
                }
            }
        }
    }
}

// ============================================================
// 子弹更新
// ============================================================
void Game::UpdateBullets(float dt) {
    // 清理过期的激光（每帧重新发射）
    for (auto& b : m_playerBullets) {
        if (b.IsActive()) {
            if (b.IsLaser()) {
                b.ReduceLaserLife(dt);
                if (b.GetLaserLife() <= 0.0f) {
                    b.Deactivate();
                } else {
                    // 激光跟随玩家
                    b.SetPosition(m_player.GetFirePosX(), m_player.GetFirePosY() - 30.0f);
                }
            } else {
                b.Update(dt);
            }
        }
    }

    for (auto& b : m_enemyBullets) {
        if (b.IsActive()) {
            b.Update(dt);
        }
    }
}

// ============================================================
// 道具更新
// ============================================================
void Game::UpdatePowerUps(float dt) {
    for (auto& p : m_powerUps) {
        if (p->IsActive()) {
            p->Update(dt);
        }
    }
}

// ============================================================
// 碰撞检测
// ============================================================
void Game::ProcessCollisions() {
    // 1. 玩家子弹 vs 敌人
    for (auto& bullet : m_playerBullets) {
        if (!bullet.IsActive()) continue;

        for (auto& enemy : m_enemies) {
            if (!enemy->IsActive() || enemy->IsDying()) continue;

            if (bullet.CollidesWith(*enemy)) {
                bullet.Deactivate();
                enemy->TakeDamage(bullet.GetDamage());

                if (enemy->IsDead() || enemy->IsDying()) {
                    m_scoreManager.AddScore(enemy->GetScoreValue());
                    m_scoreManager.AddGold(enemy->GetScoreValue() / 10);  // 金币=分数/10
                    m_scoreManager.AddCombo();

                    if (enemy->GetKind() == EnemyKind::BOSS) {
                        // Boss 爆炸由 FixedUpdate 中的 Boss 检查处理
                    } else {
                        m_particleSystem.EmitExplosion(
                            enemy->GetX(), enemy->GetY(), 25.0f, 25);
                        m_powerUpSystem.TryDrop(
                            enemy->GetX(), enemy->GetY(), false, m_powerUps);
                    }
                } else {
                    // 受击火花
                    m_particleSystem.EmitSparks(
                        bullet.GetX(), bullet.GetY(), 5);
                }
                break;
            }
        }
    }

    // 2. 敌方子弹 vs 玩家
    for (auto& bullet : m_enemyBullets) {
        if (!bullet.IsActive()) continue;

        if (m_player.IsActive() && bullet.CollidesWith(m_player)) {
            bullet.Deactivate();
            m_player.TakeDamage(bullet.GetDamage());
            m_scoreManager.ResetCombo();

            m_particleSystem.EmitExplosion(
                m_player.GetX(), m_player.GetY(), 20.0f, 15);

            if (!m_player.IsAlive()) {
                m_player.Deactivate();
                return;
            }
        }
    }

    // 3. 玩家 vs 敌人（碰撞伤害）
    if (m_player.IsActive() && !m_player.IsInvincible()) {
        for (auto& enemy : m_enemies) {
            if (!enemy->IsActive() || enemy->IsDying()) continue;
            if (!enemy->IsOnScreen(0.0f)) continue;

            if (m_player.CollidesWith(*enemy)) {
                m_player.TakeDamage(1);
                enemy->TakeDamage(enemy->GetHP());
                m_scoreManager.ResetCombo();

                m_particleSystem.EmitExplosion(
                    enemy->GetX(), enemy->GetY(), 25.0f, 20);

                if (!m_player.IsAlive()) {
                    m_player.Deactivate();
                    return;
                }
                break;
            }
        }
    }

    // 4. 玩家 vs 道具
    for (auto& powerUp : m_powerUps) {
        if (!powerUp->IsActive()) continue;

        if (m_player.IsActive() && m_player.CollidesWith(*powerUp)) {
            m_powerUpSystem.Apply(m_player, powerUp->GetPowerUpType());
            m_scoreManager.AddScore(Config::SCORE_POWERUP);
            powerUp->Deactivate();

            m_particleSystem.EmitSparks(powerUp->GetX(), powerUp->GetY(), 8);
        }
    }
}

// ============================================================
// 清理
// ============================================================
void Game::CleanupDeadEntities() {
    // 清理死亡敌人
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
                       [](auto& e) { return !e->IsActive(); }),
        m_enemies.end()
    );

    // 清理死亡道具
    m_powerUps.erase(
        std::remove_if(m_powerUps.begin(), m_powerUps.end(),
                       [](auto& p) { return !p->IsActive(); }),
        m_powerUps.end()
    );
}

// ============================================================
// 关卡过渡检查
// ============================================================
void Game::CheckLevelTransition() {
    if (m_levelManager.IsAllLevelsComplete()) {
        // 全部通关！
        m_uiManager.SetState(GameState::GAME_OVER);
        return;
    }

    if (m_levelManager.IsLevelComplete() && m_levelManager.IsInTransition()) {
        m_uiManager.SetState(GameState::LEVEL_TRANSITION);
    }
}

// ============================================================
// 渲染
// ============================================================
void Game::Render(double /*interpolationAlpha*/) {
    m_renderer.BeginFrame();

    Gdiplus::Graphics* g = m_renderer.GetGraphics();

    // Layer 0: 星空背景
    m_starField.Render(*g);

    GameState gs = m_uiManager.GetState();
    if (gs == GameState::PLAYING || gs == GameState::PAUSED
        || gs == GameState::LEVEL_TRANSITION) {

        // Layer 1: 道具
        for (auto& p : m_powerUps) {
            if (p->IsActive()) p->Render(*g);
        }

        // Layer 2: 敌人
        for (auto& e : m_enemies) {
            if (e->IsActive()) e->Render(*g);
        }

        // Layer 3: 玩家
        if (m_player.IsActive()) {
            m_player.Render(*g);
        }

        // Layer 4: 子弹
        for (auto& b : m_playerBullets) {
            if (b.IsActive()) b.Render(*g);
        }
        for (auto& b : m_enemyBullets) {
            if (b.IsActive()) b.Render(*g);
        }

        // Layer 5: 粒子特效
        m_particleSystem.Render(*g);
    }

    // Layer 6: UI 覆盖层
    m_uiManager.Render(*g, m_player, m_scoreManager, m_levelManager);

    m_renderer.EndFrame();
}

// ============================================================
// 关闭
// ============================================================
void Game::Shutdown() {
    m_isRunning = false;

    m_scoreManager.SaveProgress();
    m_scoreManager.SaveHighScores();

    m_enemies.clear();
    m_powerUps.clear();
    m_playerBullets.ReleaseAll();
    m_enemyBullets.ReleaseAll();
    m_particlePool.ReleaseAll();

    m_renderer.Shutdown();
}
