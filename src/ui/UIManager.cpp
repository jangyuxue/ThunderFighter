#include "ui/UIManager.h"
#include "config/GameConfig.h"
#include <cstdio>

// 任务/成就 helper 函数
static int GetMissionTarget(int i) {
    int t[5] = { 50, 1, 10, 1, 1 };
    return t[i];
}
static int GetMissionReward(int i) {
    int r[5] = { 100, 300, 50, 200, 80 };
    return r[i];
}

UIManager::UIManager() {
    // 字体在 Init() 中延迟创建（需要 GDI+ 已初始化）
}

UIManager::~UIManager() {
    delete m_titleFont;
    delete m_menuFont;
    delete m_buttonFont;
    delete m_smallFont;
    delete m_hudFont;
}

void UIManager::Init() {
    m_titleFont  = new Gdiplus::Font(L"Arial", 34, Gdiplus::FontStyleBold);
    m_menuFont   = new Gdiplus::Font(L"Arial", 20, Gdiplus::FontStyleBold);
    m_buttonFont = new Gdiplus::Font(L"Arial", 15, Gdiplus::FontStyleRegular);
    m_smallFont  = new Gdiplus::Font(L"Arial", 12, Gdiplus::FontStyleRegular);
    m_hudFont    = new Gdiplus::Font(L"Consolas", 14, Gdiplus::FontStyleBold);

    float cx = Config::CANVAS_WIDTH * 0.5f;

    // HUB 按钮布局：3行 x 2列
    float btnW = 150.0f, btnH = 55.0f;
    float gapX = 20.0f, gapY = 15.0f;
    float leftX  = cx - btnW * 0.5f - gapX * 0.5f - btnW;
    float rightX = cx + btnW * 0.5f + gapX * 0.5f;

    // 行1: 开始游戏 | 关卡选择
    // 行2: 商店 | 任务
    // 行3: 成就 | 最高分
    const wchar_t* labels[6] = { L"START GAME", L"LEVEL SELECT", L"SHOP", L"MISSIONS", L"ACHIEVEMENTS", L"HIGH SCORES" };
    float xs[6] = { leftX, rightX, leftX, rightX, leftX, rightX };
    float baseY = 370.0f;
    float ys[6] = { baseY, baseY, baseY + btnH + gapY, baseY + btnH + gapY,
                    baseY + (btnH + gapY) * 2, baseY + (btnH + gapY) * 2 };

    m_hubButtons.resize(6);
    for (int i = 0; i < 6; ++i) {
        m_hubButtons[i].Init(xs[i], ys[i], btnW, btnH, labels[i],
            Gdiplus::Color(200, 20, 20, 50),
            Gdiplus::Color(220, 40, 40, 90),
            Gdiplus::Color(255, 255, 255, 220));
    }

    // 战机选择箭头
    m_aircraftLeft.Init(cx - 80, 300, 36, 36, L"◀",
        Gdiplus::Color(180, 30, 30, 50), Gdiplus::Color(220, 60, 60, 100),
        Gdiplus::Color(255, 255, 220, 80));
    m_aircraftRight.Init(cx + 80, 300, 36, 36, L"▶",
        Gdiplus::Color(180, 30, 30, 50), Gdiplus::Color(220, 60, 60, 100),
        Gdiplus::Color(255, 255, 220, 80));

    // 关卡选择按钮
    m_levelButtons.resize(5);
    for (int i = 0; i < 5; ++i) {
        wchar_t buf[32];
        swprintf(buf, 32, L"LEVEL %d", i + 1);
        float ly = 200.0f + i * 72.0f;
        m_levelButtons[i].Init(cx, ly, 220, 50, buf,
            Gdiplus::Color(200, 20, 20, 50),
            Gdiplus::Color(220, 40, 40, 90));
    }

    // 商店按钮
    const wchar_t* shopLabels[8] = {
        L"Extra Life +1    100G",
        L"Weapon LV2 Start 200G",
        L"Start Shield     150G",
        L"Extra Bomb x2     50G",
        L"Speed Boost Perm 300G",
        L"Max Lives +1     250G",
        L"Bomb Start +1     80G",
        L"Shield Start +1  120G"
    };
    m_shopButtons.resize(8);
    for (int i = 0; i < 8; ++i) {
        float sy = 160.0f + i * 48.0f;
        m_shopButtons[i].Init(cx, sy, 300, 40, shopLabels[i],
            Gdiplus::Color(200, 20, 20, 50),
            Gdiplus::Color(220, 40, 40, 90));
    }

    // 任务按钮（5个任务 + 领取按钮）
    const wchar_t* missionLabels[5] = {
        L"Kill 50 enemies        100G",
        L"Defeat 1 Boss          300G",
        L"Collect 10 power-ups    50G",
        L"Clear any level        200G",
        L"Use bomb 1 time         80G"
    };
    m_missionButtons.resize(5);
    for (int i = 0; i < 5; ++i) {
        float my = 170.0f + i * 55.0f;
        m_missionButtons[i].Init(cx, my, 320, 42, missionLabels[i],
            Gdiplus::Color(200, 20, 20, 50),
            Gdiplus::Color(220, 40, 40, 90));
    }

    // 成就按钮
    const wchar_t* achievementLabels[8] = {
        L"First Victory - Clear any level",
        L"Bullet Hell Master - Beat boss unharmed",
        L"Gold Hoarder - Earn 10000 total gold",
        L"Weapon Master - Reach LV5 Laser",
        L"Exterminator - Kill 100 in one game",
        L"Phoenix - Clear a level without dying",
        L"Collector - Get all power-up types",
        L"5-Star General - Clear all levels"
    };
    m_achievementButtons.resize(8);
    for (int i = 0; i < 8; ++i) {
        float ay = 140.0f + i * 54.0f;
        m_achievementButtons[i].Init(cx, ay, 360, 42, achievementLabels[i],
            Gdiplus::Color(200, 20, 20, 50),
            Gdiplus::Color(220, 40, 40, 90));
    }

    // 返回按钮
    m_backButton.Init(60, Config::CANVAS_HEIGHT - 40, 100, 36, L"< BACK",
        Gdiplus::Color(180, 40, 40, 60), Gdiplus::Color(220, 60, 60, 100),
        Gdiplus::Color(200, 200, 200, 200));

    // 初始解锁第一关
    m_levelUnlocked[0] = true;
}

// ============================================================
// 状态管理
// ============================================================
void UIManager::SetState(GameState state) {
    m_prevState = m_state;
    m_state = state;
    m_transitionAlpha = 0.0f;
    m_backTimer = 0.3f;  // 防止误触 ESC
}

// ============================================================
// 输入处理
// ============================================================
void UIManager::HandleInput(InputManager& input, Player& /*player*/, ScoreManager& score) {
    int mx = input.GetMouseX();
    int my = input.GetMouseY();
    bool md = input.IsMouseDown();
    bool mp = input.IsMousePressed();

    if (m_backTimer > 0.0f) return;  // 冷却中

    // ESC 全局返回
    if (input.IsBackPressed() && m_state != GameState::HUB
        && m_state != GameState::PLAYING && m_state != GameState::GAME_OVER) {
        m_backTimer = 0.3f;
        SetState(m_prevState != m_state ? m_prevState : GameState::HUB);
        return;
    }

    switch (m_state) {
    case GameState::HUB:
        UpdateHubButtons(mx, my, md, mp);

        // 键盘操作（Enter 开始游戏）
        if (input.IsConfirmPressed() && !m_gameStarted) {
            m_gameStarted = true;
            m_shouldStartGame = true;
        }
        break;

    case GameState::PLAYING:
        if (input.IsPausePressed()) SetState(GameState::PAUSED);
        break;

    case GameState::PAUSED:
        if (input.IsPausePressed() || input.IsBackPressed())
            SetState(GameState::PLAYING);
        break;

    case GameState::LEVEL_SELECT:
        UpdateLevelSelectButtons(mx, my, md, mp);
        if (input.IsBackPressed()) SetState(GameState::HUB);
        break;

    case GameState::SHOP:
        UpdateShopButtons(mx, my, md, mp, score);
        if (input.IsBackPressed()) SetState(GameState::HUB);
        break;

    case GameState::MISSIONS:
        UpdateMissionsButtons(mx, my, md, mp, score);
        if (input.IsBackPressed()) SetState(GameState::HUB);
        break;

    case GameState::ACHIEVEMENTS:
        UpdateAchievementsButtons(mx, my, md, mp);
        if (input.IsBackPressed()) SetState(GameState::HUB);
        break;

    case GameState::GAME_OVER:
        if (input.IsConfirmPressed() || input.IsMousePressed()) {
            m_gameStarted = false;
            m_shouldStartGame = false;
            SetState(GameState::HUB);
        }
        break;

    case GameState::HIGH_SCORE:
        if (input.IsBackPressed()) SetState(GameState::HUB);
        break;

    default: break;
    }
}

void UIManager::Update(float dt) {
    m_menuTimer += dt;
    if (m_backTimer > 0.0f) m_backTimer -= dt;
    if (m_state == GameState::LEVEL_TRANSITION) {
        m_transitionAlpha -= dt * 0.5f;
        if (m_transitionAlpha < 0.0f) m_transitionAlpha = 0.0f;
    }
    if (m_achievementNotifyTimer > 0.0f) {
        m_achievementNotifyTimer -= dt;
        if (m_achievementNotifyTimer < 0.0f) {
            m_achievementNotifyId = -1;
        }
    }
}

// ============================================================
// 按钮更新
// ============================================================
void UIManager::UpdateHubButtons(int mx, int my, bool down, bool pressed) {
    for (auto& btn : m_hubButtons) btn.Update(mx, my, down, pressed);
    m_aircraftLeft.Update(mx, my, down, pressed);
    m_aircraftRight.Update(mx, my, down, pressed);

    if (m_aircraftLeft.IsClicked()) {
        m_selectedAircraft = (m_selectedAircraft + 2) % 3;
    }
    if (m_aircraftRight.IsClicked()) {
        m_selectedAircraft = (m_selectedAircraft + 1) % 3;
    }

    // 按钮点击 → 切换界面
    if (m_hubButtons[0].IsClicked()) { m_gameStarted = true; m_shouldStartGame = true; }
    if (m_hubButtons[1].IsClicked()) SetState(GameState::LEVEL_SELECT);
    if (m_hubButtons[2].IsClicked()) SetState(GameState::SHOP);
    if (m_hubButtons[3].IsClicked()) SetState(GameState::MISSIONS);
    if (m_hubButtons[4].IsClicked()) SetState(GameState::ACHIEVEMENTS);
    if (m_hubButtons[5].IsClicked()) SetState(GameState::HIGH_SCORE);
}

void UIManager::UpdateLevelSelectButtons(int mx, int my, bool down, bool pressed) {
    m_backButton.Update(mx, my, down, pressed);
    if (m_backButton.IsClicked()) SetState(GameState::HUB);

    for (int i = 0; i < 5; ++i) {
        m_levelButtons[i].SetLocked(!m_levelUnlocked[i]);
        m_levelButtons[i].Update(mx, my, down, pressed);
        if (m_levelButtons[i].IsClicked() && m_levelUnlocked[i]) {
            m_selectedLevel = i + 1;
            m_gameStarted = true;
            m_shouldStartGame = true;
        }
    }
}

void UIManager::UpdateShopButtons(int mx, int my, bool down, bool pressed, ScoreManager& score) {
    m_backButton.Update(mx, my, down, pressed);
    if (m_backButton.IsClicked()) SetState(GameState::HUB);

    for (int i = 0; i < 8; ++i) {
        m_shopButtons[i].SetOwned(m_shopOwned[i]);
        m_shopButtons[i].Update(mx, my, down, pressed);
        if (m_shopButtons[i].IsClicked() && !m_shopOwned[i]) {
            int cost = GetShopItemCost(i);
            if (score.GetGold() >= cost) {
                score.SpendGold(cost);
                BuyShopItem(i, score);
            }
        }
    }
}

void UIManager::UpdateMissionsButtons(int mx, int my, bool down, bool pressed, ScoreManager& score) {
    m_backButton.Update(mx, my, down, pressed);
    if (m_backButton.IsClicked()) SetState(GameState::HUB);

    for (int i = 0; i < 5; ++i) {
        bool claimed = m_missionClaimed[i];
        bool complete = m_missionProgress[i] >= GetMissionTarget(i);
        m_missionButtons[i].SetOwned(claimed);
        m_missionButtons[i].Update(mx, my, down, pressed);
        if (m_missionButtons[i].IsClicked() && complete && !claimed) {
            m_missionClaimed[i] = true;
            score.AddGold(GetMissionReward(i));
        }
    }
}

void UIManager::UpdateAchievementsButtons(int mx, int my, bool down, bool pressed) {
    m_backButton.Update(mx, my, down, pressed);
    if (m_backButton.IsClicked()) SetState(GameState::HUB);
}

// ============================================================
// 商店逻辑
// ============================================================
bool UIManager::HasShopItem(int id) const { return m_shopOwned[id]; }
int UIManager::GetShopItemCost(int id) const {
    int costs[8] = { 100, 200, 150, 50, 300, 250, 80, 120 };
    return costs[id];
}
void UIManager::BuyShopItem(int id, ScoreManager& /*score*/) {
    m_shopOwned[id] = true;
    if (id == 0) m_playerUpgrades |= 1;   // extra life
    if (id == 1) m_playerUpgrades |= 2;   // weapon lv2
    if (id == 2) m_playerUpgrades |= 4;   // start shield
    if (id == 4) m_playerUpgrades |= 8;   // speed boost
}

// ============================================================
// 渲染主调度
// ============================================================
void UIManager::Render(Gdiplus::Graphics& g,
                       Player& player,
                       ScoreManager& score,
                       const LevelManager& level) {
    switch (m_state) {
    case GameState::HUB:          RenderHub(g, score); break;
    case GameState::PLAYING:      RenderHUD(g, player, score, level); break;
    case GameState::PAUSED:       RenderHUD(g, player, score, level); RenderPaused(g); break;
    case GameState::LEVEL_SELECT: RenderLevelSelect(g, score); break;
    case GameState::SHOP:         RenderShop(g, score); break;
    case GameState::MISSIONS:     RenderMissions(g, score); break;
    case GameState::ACHIEVEMENTS: RenderAchievements(g); break;
    case GameState::LEVEL_TRANSITION: RenderLevelTransition(g, level); break;
    case GameState::GAME_OVER:    RenderGameOver(g, player, score); break;
    case GameState::HIGH_SCORE:   RenderHighScore(g, score); break;
    }
}

// ============================================================
// HUB 大厅界面
// ============================================================
void UIManager::RenderHub(Gdiplus::Graphics& g, ScoreManager& score) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    // 标题
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"THUNDER FIGHTER", -1, m_titleFont,
                 Gdiplus::PointF(cx, 70), &fmt, &titleBrush);

    Gdiplus::SolidBrush subBrush(Gdiplus::Color(200, 180, 180, 200));
    g.DrawString(L"雷霆战机", -1, m_menuFont,
                 Gdiplus::PointF(cx, 115), &fmt, &subBrush);

    // 战机选择
    const wchar_t* names[3] = { L"LIGHTNING  SPEED", L"HURRICANE  BALANCED", L"INFERNO  POWER" };
    Gdiplus::Color colors[3] = {
        Gdiplus::Color(255, 80, 160, 255),
        Gdiplus::Color(255, 60, 220, 80),
        Gdiplus::Color(255, 255, 60, 60)
    };
    Gdiplus::SolidBrush acBrush(colors[m_selectedAircraft]);
    g.DrawString(names[m_selectedAircraft], -1, m_menuFont,
                 Gdiplus::PointF(cx, 260), &fmt, &acBrush);

    // 战绩描述
    const wchar_t* descs[3] = {
        L"Fast & Agile | Single Shot | 2 HP | Blue",
        L"Balanced All-Rounder | Standard | 3 HP | Green",
        L"Heavy Firepower | Double Shot | 4 HP | Red"
    };
    Gdiplus::SolidBrush descBrush(Gdiplus::Color(150, 160, 160, 180));
    g.DrawString(descs[m_selectedAircraft], -1, m_smallFont,
                 Gdiplus::PointF(cx, 335), &fmt, &descBrush);

    // 战机选择箭头
    m_aircraftLeft.Render(g, *m_buttonFont);
    m_aircraftRight.Render(g, *m_buttonFont);

    // 6个功能按钮
    for (auto& btn : m_hubButtons) btn.Render(g, *m_buttonFont);

    // 金币显示
    Gdiplus::SolidBrush goldBrush(Gdiplus::Color(255, 255, 220, 80));
    wchar_t buf[64];
    swprintf(buf, 64, L"GOLD: %d", score.GetGold());
    g.DrawString(buf, -1, m_menuFont,
                 Gdiplus::PointF(cx, Config::CANVAS_HEIGHT - 30), &fmt, &goldBrush);

    // 底部提示
    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(140, 140, 140, 160));
    g.DrawString(L"Click or Enter to start | ESC to exit", -1, m_smallFont,
                 Gdiplus::PointF(cx, Config::CANVAS_HEIGHT - 10), &fmt, &hintBrush);
}

// ============================================================
// 关卡选择界面
// ============================================================
void UIManager::RenderLevelSelect(Gdiplus::Graphics& g, ScoreManager& /*score*/) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    // 标题
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"LEVEL SELECT", -1, m_menuFont,
                 Gdiplus::PointF(cx, 40), &fmt, &titleBrush);

    const char* levelNames[5] = {
        "Earth Orbit", "Asteroid Belt", "Enemy Fleet",
        "Deep Space Fortress", "Mothership"
    };

    for (int i = 0; i < 5; ++i) {
        wchar_t buf[128];
        const char* status;
        if (m_levelCleared[i]) status = "[CLEARED]";
        else if (m_levelUnlocked[i]) status = "[UNLOCKED]";
        else status = "[LOCKED]";

        swprintf(buf, 128, L"Level %d: %hs  %hs", i + 1, levelNames[i], status);
        Gdiplus::SolidBrush txtBrush(m_levelUnlocked[i]
            ? Gdiplus::Color(200, 200, 200, 200)
            : Gdiplus::Color(100, 80, 80, 80));
        g.DrawString(buf, -1, m_smallFont,
                     Gdiplus::PointF(cx, 140.0f + i * 60.0f), &fmt, &txtBrush);

        m_levelButtons[i].Render(g, *m_buttonFont);
    }

    m_backButton.Render(g, *m_smallFont);
}

// ============================================================
// 商店界面
// ============================================================
void UIManager::RenderShop(Gdiplus::Graphics& g, ScoreManager& score) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"SHOP", -1, m_menuFont, Gdiplus::PointF(cx, 40), &fmt, &titleBrush);

    wchar_t buf[64];
    swprintf(buf, 64, L"Your Gold: %d", score.GetGold());
    Gdiplus::SolidBrush goldBrush(Gdiplus::Color(200, 255, 220, 80));
    g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(cx, 75), &fmt, &goldBrush);

    for (int i = 0; i < 8; ++i) {
        if (m_shopOwned[i]) {
            Gdiplus::SolidBrush ownedBrush(Gdiplus::Color(150, 80, 220, 80));
            g.DrawString(L"OWNED", -1, m_smallFont,
                         Gdiplus::PointF(cx + 140, 160.0f + i * 48.0f), &fmt, &ownedBrush);
        }
        m_shopButtons[i].Render(g, *m_buttonFont);
    }

    m_backButton.Render(g, *m_smallFont);
}

// ============================================================
// 任务界面
// ============================================================
void UIManager::RenderMissions(Gdiplus::Graphics& g, ScoreManager& /*score*/) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"MISSIONS", -1, m_menuFont, Gdiplus::PointF(cx, 40), &fmt, &titleBrush);

    for (int i = 0; i < 5; ++i) {
        wchar_t buf[64];
        if (m_missionClaimed[i]) {
            swprintf(buf, 64, L"COMPLETED!");
        } else {
            int prog = m_missionProgress[i];
            int target = (i == 0) ? 50 : (i == 1) ? 1 : (i == 2) ? 10 : (i == 3) ? 1 : 1;
            swprintf(buf, 64, L"Progress: %d/%d", prog, target);
        }
        Gdiplus::SolidBrush progBrush(m_missionClaimed[i]
            ? Gdiplus::Color(150, 80, 220, 80)
            : Gdiplus::Color(180, 200, 200, 200));
        g.DrawString(buf, -1, m_smallFont,
                     Gdiplus::PointF(cx, 170.0f + i * 55.0f + 22), &fmt, &progBrush);

        m_missionButtons[i].Render(g, *m_buttonFont);
    }

    m_backButton.Render(g, *m_smallFont);
}

// ============================================================
// 成就界面
// ============================================================
void UIManager::RenderAchievements(Gdiplus::Graphics& g) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"ACHIEVEMENTS", -1, m_menuFont, Gdiplus::PointF(cx, 40), &fmt, &titleBrush);

    for (int i = 0; i < 8; ++i) {
        Gdiplus::Color statusColor = m_achievementUnlocked[i]
            ? Gdiplus::Color(200, 255, 220, 80)
            : Gdiplus::Color(100, 80, 80, 80);
        Gdiplus::SolidBrush statusBrush(statusColor);
        g.DrawString(m_achievementUnlocked[i] ? L"UNLOCKED" : L"LOCKED", -1, m_smallFont,
                     Gdiplus::PointF(cx + 170, 140.0f + i * 54.0f + 22), &fmt, &statusBrush);

        m_achievementButtons[i].Render(g, *m_buttonFont);
    }

    m_backButton.Render(g, *m_smallFont);
}

// ============================================================
// 游戏内 HUD
// ============================================================
void UIManager::RenderHUD(Gdiplus::Graphics& g,
                          Player& player,
                          ScoreManager& score,
                          const LevelManager& level) {
    Gdiplus::StringFormat fmtL;
    fmtL.SetAlignment(Gdiplus::StringAlignmentNear);

    wchar_t buf[64];
    // 分数
    swprintf(buf, 64, L"SCORE: %u  GOLD: %d", score.GetScore(), score.GetGold());
    Gdiplus::SolidBrush scoreBrush(Gdiplus::Color(255, 255, 255, 200));
    g.DrawString(buf, -1, m_hudFont, Gdiplus::PointF(10, 5), &fmtL, &scoreBrush);

    // 关卡名
    Gdiplus::StringFormat fmtR;
    fmtR.SetAlignment(Gdiplus::StringAlignmentFar);
    swprintf(buf, 64, L"LEVEL %d", level.GetCurrentLevel());
    Gdiplus::SolidBrush levelBrush(Gdiplus::Color(200, 200, 200, 200));
    g.DrawString(buf, -1, m_menuFont, Gdiplus::PointF(Config::CANVAS_WIDTH - 10, 5), &fmtR, &levelBrush);

    // 底部状态
    float lifeY = Config::CANVAS_HEIGHT - 35;
    swprintf(buf, 64, L"HP: %d  BOMB: %d  WPN: LV%d",
             player.GetLives(), player.GetBombs(), player.GetWeaponLevel() + 1);
    Gdiplus::SolidBrush statusBrush(Gdiplus::Color(200, 200, 255, 200));
    g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(10, lifeY), &fmtL, &statusBrush);
}

// ============================================================
// 暂停/过渡/结束/最高分（保持简洁）
// ============================================================
void UIManager::RenderPaused(Gdiplus::Graphics& g) {
    Gdiplus::SolidBrush overlay(Gdiplus::Color(150, 0, 0, 0));
    g.FillRectangle(&overlay, 0.0f, 0.0f,
        static_cast<float>(Config::CANVAS_WIDTH),
        static_cast<float>(Config::CANVAS_HEIGHT));

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::SolidBrush pauseBrush(Gdiplus::Color(255, 255, 255, 200));
    g.DrawString(L"PAUSED", -1, m_titleFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, Config::CANVAS_HEIGHT * 0.5f - 20),
                 &fmt, &pauseBrush);
    g.DrawString(L"Press SPACE to continue", -1, m_smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, Config::CANVAS_HEIGHT * 0.5f + 30),
                 &fmt, &pauseBrush);
}

void UIManager::RenderLevelTransition(Gdiplus::Graphics& g, const LevelManager& level) {
    float alpha = m_transitionAlpha;
    if (alpha <= 0.0f) return;
    Gdiplus::SolidBrush overlay(Gdiplus::Color(static_cast<BYTE>(200 * alpha), 0, 0, 0));
    g.FillRectangle(&overlay, 0.0f, 0.0f,
        static_cast<float>(Config::CANVAS_WIDTH), static_cast<float>(Config::CANVAS_HEIGHT));

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    wchar_t buf[128];
    swprintf(buf, 128, L"LEVEL %d COMPLETE!", level.GetCurrentLevel() - 1);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(static_cast<BYTE>(255 * alpha), 255, 255, 100));
    g.DrawString(buf, -1, m_menuFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, Config::CANVAS_HEIGHT * 0.5f),
                 &fmt, &textBrush);
}

void UIManager::RenderGameOver(Gdiplus::Graphics& g, Player& /*player*/, ScoreManager& score) {
    Gdiplus::SolidBrush overlay(Gdiplus::Color(180, 10, 0, 0));
    g.FillRectangle(&overlay, 0.0f, 0.0f,
        static_cast<float>(Config::CANVAS_WIDTH), static_cast<float>(Config::CANVAS_HEIGHT));

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    float cx = Config::CANVAS_WIDTH * 0.5f;

    Gdiplus::SolidBrush overBrush(Gdiplus::Color(255, 255, 60, 60));
    g.DrawString(L"GAME OVER", -1, m_titleFont, Gdiplus::PointF(cx, 200), &fmt, &overBrush);

    wchar_t buf[64];
    swprintf(buf, 64, L"Final Score: %u  Gold earned: %d", score.GetScore(), score.GetGold());
    Gdiplus::SolidBrush scoreBrush(Gdiplus::Color(255, 255, 255, 200));
    g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(cx, 280), &fmt, &scoreBrush);

    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(200, 180, 180, 180));
    g.DrawString(L"Click or press ENTER to return to hub", -1, m_smallFont,
                 Gdiplus::PointF(cx, 380), &fmt, &hintBrush);
}

void UIManager::RenderHighScore(Gdiplus::Graphics& g, ScoreManager& score) {
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(200, 5, 5, 18));
    g.FillRectangle(&bgBrush, 0.0f, 0.0f,
        static_cast<float>(Config::CANVAS_WIDTH), static_cast<float>(Config::CANVAS_HEIGHT));

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    float cx = Config::CANVAS_WIDTH * 0.5f;

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"HIGH SCORES", -1, m_menuFont, Gdiplus::PointF(cx, 50), &fmt, &titleBrush);

    wchar_t buf[128];
    for (int i = 0; i < 10; ++i) {
        float y = 110.0f + i * 35.0f;
        const auto& entry = score.GetHighScores()[i];
        if (entry.score > 0)
            swprintf(buf, 128, L"%2d. %hs  %u", i + 1, entry.name, entry.score);
        else
            swprintf(buf, 128, L"%2d.  ---", i + 1);
        Gdiplus::SolidBrush entryBrush(Gdiplus::Color(200, 200, 200, 200));
        g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(cx, y), &fmt, &entryBrush);
    }

    Gdiplus::SolidBrush backBrush(Gdiplus::Color(180, 160, 160, 160));
    g.DrawString(L"Press ESC to return", -1, m_smallFont, Gdiplus::PointF(cx, 500), &fmt, &backBrush);
}

