#include "ui/UIManager.h"
#include "config/GameConfig.h"
#include <cstdio>

UIManager::UIManager() {}
UIManager::~UIManager() {
    delete m_titleFont; delete m_menuFont;
    delete m_buttonFont; delete m_smallFont; delete m_hudFont;
}

// 获取系统默认中文字体名称（绝对可靠）
static std::wstring GetSystemFontName() {
    // 方法1: 从 DEFAULT_GUI_FONT 获取系统默认字体名
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
        return ncm.lfMessageFont.lfFaceName;  // 系统消息字体，中文Windows必然是中文
    }
    // 方法2: GetStockObject
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    if (hFont) {
        LOGFONTW lf = {};
        GetObjectW(hFont, sizeof(LOGFONTW), &lf);
        if (lf.lfFaceName[0]) return lf.lfFaceName;
    }
    return L"Microsoft YaHei";  // 最终fallback
}

void UIManager::Init() {
    m_fontName = GetSystemFontName();

    m_titleFont  = new Gdiplus::Font(m_fontName.c_str(), 24, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    m_menuFont   = new Gdiplus::Font(m_fontName.c_str(), 16, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
    m_buttonFont = new Gdiplus::Font(m_fontName.c_str(), 12, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    m_smallFont  = new Gdiplus::Font(m_fontName.c_str(), 10, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    m_hudFont    = new Gdiplus::Font(m_fontName.c_str(), 11, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);

    float cx = Config::CANVAS_WIDTH * 0.5f;

    // HUB 按钮：3个按钮垂直居中排列（已移除成就、排行榜、任务）
    float btnW = 180.0f, btnH = 54.0f;
    float gapY = 20.0f;
    float baseY  = 430.0f;

    const wchar_t* hubLabels[3] = {
        L"开始游戏", L"关卡选择", L"商  店"
    };
    float hx = cx;
    m_hubButtons.resize(3);
    for (int i = 0; i < 3; ++i) {
        float hy = baseY + i * (btnH + gapY);
        m_hubButtons[i].Init(hx, hy, btnW, btnH, hubLabels[i],
            Gdiplus::Color(200, 15, 20, 45),
            Gdiplus::Color(230, 35, 60, 100),
            Gdiplus::Color(255, 255, 240, 200));
    }

    // 战机选择箭头
    m_aircraftLeft.Init(cx - 100, 310, 40, 40, L"◀",
        Gdiplus::Color(160, 20, 30, 40), Gdiplus::Color(220, 50, 80, 120));
    m_aircraftRight.Init(cx + 100, 310, 40, 40, L"▶",
        Gdiplus::Color(160, 20, 30, 40), Gdiplus::Color(220, 50, 80, 120));

    // 关卡选择按钮
    m_levelButtons.resize(5);
    for (int i = 0; i < 5; ++i) {
        wchar_t buf[64];
        swprintf(buf, 64, L"第%d关", i + 1);
        float ly = 150.0f + i * 68.0f;
        m_levelButtons[i].Init(cx, ly, 240, 50, buf,
            Gdiplus::Color(200, 15, 20, 45), Gdiplus::Color(230, 35, 60, 100));
    }

    // 商店按钮
    const wchar_t* shopLabels[8] = {
        L"额外生命 +1      金币100",
        L"武器预升级        金币200",
        L"起始护盾          金币150",
        L"额外炸弹 x2       金币50",
        L"永久加速          金币300",
        L"最大生命 +1       金币250",
        L"起始炸弹 +1       金币80",
        L"起始护盾 +1       金币120"
    };
    m_shopButtons.resize(8);
    for (int i = 0; i < 8; ++i) {
        float sy = 140.0f + i * 46.0f;
        m_shopButtons[i].Init(cx, sy, 340, 38, shopLabels[i],
            Gdiplus::Color(200, 15, 20, 45), Gdiplus::Color(230, 35, 60, 100));
    }

    // 返回按钮（加大加粗，确保可见可点击）
    m_backButton.Init(70, Config::CANVAS_HEIGHT - 46, 120, 42, L"返回",
        Gdiplus::Color(200, 40, 40, 70), Gdiplus::Color(240, 80, 80, 140),
        Gdiplus::Color(255, 255, 240, 220));

    m_levelUnlocked[0] = true;
    SetState(GameState::HUB);  // 确保每次初始化都从大厅开始
}

void UIManager::LoadData(ScoreManager& score) {
    score.LoadAll(m_shopOwned, m_levelCleared, m_levelUnlocked);
    m_levelUnlocked[0] = true;  // 第1关永远解锁
}

void UIManager::SaveData(ScoreManager& score) {
    score.SaveAll(m_shopOwned, m_levelCleared, m_levelUnlocked);
}

void UIManager::OnLevelCleared(int level) {
    if (level >= 1 && level <= 5) {
        m_levelCleared[level - 1] = true;
        if (level < 5) m_levelUnlocked[level] = true;  // 解锁下一关
    }
}

// 状态管理
void UIManager::SetState(GameState state) {
    m_prevState = m_state;
    m_state = state;
    m_backTimer = 0.3f;
    if (state == GameState::LEVEL_TRANSITION) m_transitionAlpha = 1.0f;
    else m_transitionAlpha = 0.0f;
}

void UIManager::HandleInput(InputManager& input, Player& /*player*/, ScoreManager& score) {
    int mx = input.GetMouseX(), my = input.GetMouseY();
    bool md = input.IsMouseDown(), mp = input.IsMousePressed();

    // ESC 全局返回
    if (input.IsBackPressed()) {
        if (m_state == GameState::HUB || m_state == GameState::PLAYING) {
            // HUB: ESC 无操作; PLAYING: 由下面的 PAUSE 处理
        } else if (m_state == GameState::PAUSED) {
            SetState(GameState::PLAYING);
            return;
        } else if (m_state == GameState::GAME_OVER) {
            m_shouldStartGame = false;
            SetState(GameState::HUB);
            return;
        } else {
            SetState(GameState::HUB);
            return;
        }
    }

    switch (m_state) {
    case GameState::HUB:
        UpdateHubButtons(mx, my, md, mp);
        if (input.IsKeyPressed(VK_LEFT) || input.IsKeyPressed('A'))
            m_selectedAircraft = (m_selectedAircraft + 2) % 3;
        if (input.IsKeyPressed(VK_RIGHT) || input.IsKeyPressed('D'))
            m_selectedAircraft = (m_selectedAircraft + 1) % 3;
        if (input.IsConfirmPressed()) m_shouldStartGame = true;
        break;
    case GameState::PLAYING:
        if (input.IsPausePressed()) SetState(GameState::PAUSED);
        break;
    case GameState::PAUSED:
        if (input.IsPausePressed()) SetState(GameState::PLAYING);
        break;
    case GameState::LEVEL_SELECT:
        UpdateLevelSelectButtons(mx, my, md, mp);
        break;
    case GameState::SHOP:
        UpdateShopButtons(mx, my, md, mp, score);
        break;
    case GameState::GAME_OVER:
        if (input.IsConfirmPressed() || mp) {
            m_shouldStartGame = false;
            SetState(GameState::HUB);
        }
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
}

// 按钮更新
void UIManager::UpdateHubButtons(int mx, int my, bool down, bool pressed) {
    for (auto& btn : m_hubButtons) btn.Update(mx, my, down, pressed);
    m_aircraftLeft.Update(mx, my, down, pressed);
    m_aircraftRight.Update(mx, my, down, pressed);
    if (m_aircraftLeft.IsClicked())  m_selectedAircraft = (m_selectedAircraft + 2) % 3;
    if (m_aircraftRight.IsClicked()) m_selectedAircraft = (m_selectedAircraft + 1) % 3;
    if (m_hubButtons[0].IsClicked()) m_shouldStartGame = true;
    if (m_hubButtons[1].IsClicked()) SetState(GameState::LEVEL_SELECT);
    if (m_hubButtons[2].IsClicked()) SetState(GameState::SHOP);
}

void UIManager::UpdateLevelSelectButtons(int mx, int my, bool down, bool pressed) {
    m_backButton.Update(mx, my, down, pressed);
    if (m_backButton.IsClicked()) { SetState(GameState::HUB); return; }
    for (int i = 0; i < 5; ++i) {
        m_levelButtons[i].SetLocked(!m_levelUnlocked[i]);
        m_levelButtons[i].Update(mx, my, down, pressed);
        if (m_levelButtons[i].IsClicked() && m_levelUnlocked[i]) {
            m_selectedLevel = i + 1;
            m_shouldStartGame = true;
        }
    }
}

void UIManager::UpdateShopButtons(int mx, int my, bool down, bool pressed, ScoreManager& score) {
    m_backButton.Update(mx, my, down, pressed);
    if (m_backButton.IsClicked()) { SetState(GameState::HUB); return; }
    for (int i = 0; i < 8; ++i) {
        m_shopButtons[i].SetOwned(m_shopOwned[i]);
        m_shopButtons[i].Update(mx, my, down, pressed);
        if (m_shopButtons[i].IsClicked() && !m_shopOwned[i]) {
            int cost = GetShopItemCost(i);
            if (score.GetGold() >= cost) {
                score.SpendGold(cost);
                BuyShopItem(i, score);
                SaveData(score);  // 立即持久化：金币+能力一起落盘，防止退出后丢失
            }
        }
    }
}

// 渲染调度
void UIManager::Render(Gdiplus::Graphics& g, Player& player,
                       ScoreManager& score, const LevelManager& level) {
    switch (m_state) {
    case GameState::HUB:          RenderHub(g, score); break;
    case GameState::PLAYING:      RenderHUD(g, player, score, level); break;
    case GameState::PAUSED:       RenderHUD(g, player, score, level); RenderPaused(g); break;
    case GameState::LEVEL_SELECT: RenderLevelSelect(g, score); break;
    case GameState::SHOP:         RenderShop(g, score); break;
    case GameState::LEVEL_TRANSITION: RenderLevelTransition(g, level); break;
    case GameState::GAME_OVER:    RenderGameOver(g, player, score); break;
    }
}

// ========== 大厅 ==========
void UIManager::RenderHub(Gdiplus::Graphics& g, ScoreManager& score) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    // 标题背景装饰线
    Gdiplus::Pen line1(Gdiplus::Color(120, 255, 200, 60), 2.0f);
    g.DrawLine(&line1, Gdiplus::PointF(40, 90), Gdiplus::PointF(cx - 130, 90));
    g.DrawLine(&line1, Gdiplus::PointF(cx + 130, 90), Gdiplus::PointF(440, 90));

    // 标题
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"雷 霆 战 机", -1, m_titleFont, Gdiplus::PointF(cx, 60), &fmt, &titleBrush);

    Gdiplus::SolidBrush subBrush(Gdiplus::Color(180, 140, 180, 220));
    Gdiplus::Font subFont(m_fontName.c_str(), 13);
    g.DrawString(L"THUNDER FIGHTER", -1, &subFont, Gdiplus::PointF(cx, 105), &fmt, &subBrush);

    // 分隔线
    Gdiplus::Pen line2(Gdiplus::Color(80, 255, 200, 60), 1.0f);
    g.DrawLine(&line2, Gdiplus::PointF(60, 130), Gdiplus::PointF(420, 130));

    // 战机选择标题
    Gdiplus::SolidBrush selTitle(Gdiplus::Color(180, 180, 200, 220));
    g.DrawString(L"— 选择战机 —", -1, m_smallFont, Gdiplus::PointF(cx, 155), &fmt, &selTitle);

    // 当前战机名 + 型号
    const wchar_t* names[3] = { L"闪电", L"飓风", L"烈焰" };
    const wchar_t* types[3] = { L"速度型", L"均衡型", L"力量型" };
    const wchar_t* descs[3] = {
        L"高机动 · 单发 · 2生命 · 蓝色涂装",
        L"全能 · 标准火力 · 3生命 · 绿色涂装",
        L"重火力 · 双发 · 4生命 · 红色涂装"
    };
    Gdiplus::Color typeColors[3] = {
        Gdiplus::Color(255, 80, 160, 255),
        Gdiplus::Color(255, 60, 220, 80),
        Gdiplus::Color(255, 255, 60, 60)
    };

    // 战机名
    Gdiplus::Font nameFont(m_fontName.c_str(), 24, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush nameBrush(typeColors[m_selectedAircraft]);
    wchar_t buf[64];
    swprintf(buf, 64, L"%ls  %ls", names[m_selectedAircraft], types[m_selectedAircraft]);
    g.DrawString(buf, -1, &nameFont, Gdiplus::PointF(cx, 185), &fmt, &nameBrush);

    // 描述
    Gdiplus::SolidBrush descBrush(Gdiplus::Color(160, 160, 170, 180));
    g.DrawString(descs[m_selectedAircraft], -1, m_smallFont, Gdiplus::PointF(cx, 220), &fmt, &descBrush);

    // 左右箭头
    m_aircraftLeft.Render(g, *m_buttonFont);
    m_aircraftRight.Render(g, *m_buttonFont);

    // 分隔线
    g.DrawLine(&line2, Gdiplus::PointF(60, 265), Gdiplus::PointF(420, 265));

    // 3个功能按钮（垂直居中排列）
    for (auto& btn : m_hubButtons) btn.Render(g, *m_buttonFont);

    // 底部信息
    Gdiplus::Font botFont(m_fontName.c_str(), 15, Gdiplus::FontStyleBold);
    swprintf(buf, 64, L"金币：%d", score.GetGold());
    Gdiplus::SolidBrush goldBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(buf, -1, &botFont, Gdiplus::PointF(cx, Config::CANVAS_HEIGHT - 40), &fmt, &goldBrush);

    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(120, 130, 140, 150));
    g.DrawString(L"鼠标点击按钮 · Enter 快速开始 · ESC 返回", -1, m_smallFont,
                 Gdiplus::PointF(cx, Config::CANVAS_HEIGHT - 16), &fmt, &hintBrush);
}

// ========== 关卡选择 ==========
void UIManager::RenderLevelSelect(Gdiplus::Graphics& g, ScoreManager& /*score*/) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"关卡选择", -1, m_menuFont, Gdiplus::PointF(cx, 35), &fmt, &titleBrush);

    const wchar_t* levelNames[5] = {
        L"地球轨道", L"小行星带", L"敌方舰队", L"深空要塞", L"母舰决战"
    };
    const wchar_t* diffs[5] = { L"★", L"★★", L"★★★", L"★★★★", L"★★★★★" };

    for (int i = 0; i < 5; ++i) {
        float y = 120.0f + i * 68.0f;
        const wchar_t* status;
        Gdiplus::Color statusColor;
        if (m_levelCleared[i])       { status = L"已通关"; statusColor = Gdiplus::Color(200, 80, 220, 80); }
        else if (m_levelUnlocked[i]) { status = L"已解锁"; statusColor = Gdiplus::Color(200, 255, 220, 80); }
        else                         { status = L"未解锁"; statusColor = Gdiplus::Color(100, 100, 100, 100); }

        // 关卡卡片背景
        Gdiplus::SolidBrush cardBg(m_levelUnlocked[i]
            ? Gdiplus::Color(180, 15, 20, 45) : Gdiplus::Color(150, 10, 10, 20));
        g.FillRectangle(&cardBg, cx - 140.0f, y - 8.0f, 280.0f, 56.0f);

        Gdiplus::Pen cardBorder(statusColor, 1.0f);
        g.DrawRectangle(&cardBorder, cx - 140.0f, y - 8.0f, 280.0f, 56.0f);

        // 关卡信息
        wchar_t buf[128];
        swprintf(buf, 128, L"第%d关 · %ls   %ls", i + 1, levelNames[i], diffs[i]);
        Gdiplus::SolidBrush txtBrush(m_levelUnlocked[i]
            ? Gdiplus::Color(220, 220, 220, 220) : Gdiplus::Color(80, 80, 80, 80));
        g.DrawString(buf, -1, m_buttonFont, Gdiplus::PointF(cx, y + 2), &fmt, &txtBrush);

        // 状态标签
        Gdiplus::SolidBrush statusBrush(statusColor);
        g.DrawString(status, -1, m_smallFont, Gdiplus::PointF(cx, y + 24), &fmt, &statusBrush);

        m_levelButtons[i].Render(g, *m_buttonFont);
    }

    m_backButton.Render(g, *m_buttonFont);
}

// ========== 商店 ==========
void UIManager::RenderShop(Gdiplus::Graphics& g, ScoreManager& score) {
    float cx = Config::CANVAS_WIDTH * 0.5f;
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"商  店", -1, m_menuFont, Gdiplus::PointF(cx, 30), &fmt, &titleBrush);

    wchar_t buf[128];
    swprintf(buf, 128, L"当前金币：%d", score.GetGold());
    Gdiplus::SolidBrush goldBrush(Gdiplus::Color(200, 255, 220, 80));
    g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(cx, 62), &fmt, &goldBrush);

    // 当前战机预览（按已购道具直接展示生效效果）
    const wchar_t* names[3] = { L"闪电", L"飓风", L"烈焰" };
    Gdiplus::Color typeColors[3] = {
        Gdiplus::Color(255, 80, 160, 255),
        Gdiplus::Color(255, 60, 220, 80),
        Gdiplus::Color(255, 255, 60, 60)
    };
    Gdiplus::SolidBrush acBrush(typeColors[m_selectedAircraft]);
    int lifeB = m_shopOwned[0] ? 1 : 0;                 // 额外生命
    int maxB  = m_shopOwned[5] ? 1 : 0;                 // 最大生命上限
    int wpn   = m_shopOwned[1] ? 1 : 0;                 // 武器预升级
    int shd   = (m_shopOwned[2] ? 1 : 0) + (m_shopOwned[7] ? 1 : 0); // 起始护盾
    int bmb   = (m_shopOwned[3] ? 2 : 0) + (m_shopOwned[6] ? 1 : 0); // 起始炸弹
    bool spd  = m_shopOwned[4];                         // 永久加速
    swprintf(buf, 128,
        L"战机：%ls  生命+%d(上限+%d) 武器+%d 护盾+%d 炸弹+%d 加速%ls",
        names[m_selectedAircraft], lifeB, maxB, wpn, shd, bmb,
        spd ? L"是" : L"否");
    g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(cx, 90), &fmt, &acBrush);

    Gdiplus::Pen sep(Gdiplus::Color(60, 255, 200, 60), 1.0f);
    g.DrawLine(&sep, Gdiplus::PointF(40, 110), Gdiplus::PointF(440, 110));

    for (int i = 0; i < 8; ++i) {
        m_shopButtons[i].Render(g, *m_buttonFont);
        if (m_shopOwned[i]) {
            Gdiplus::SolidBrush ownedBrush(Gdiplus::Color(180, 80, 220, 80));
            g.DrawString(L"已拥有", -1, m_smallFont,
                         Gdiplus::PointF(cx + 165, 140.0f + i * 46.0f + 2), &fmt, &ownedBrush);
        }
    }

    Gdiplus::SolidBrush tipBrush(Gdiplus::Color(120, 140, 140, 140));
    g.DrawString(L"购买的道具将在下一局游戏中生效", -1, m_smallFont,
                 Gdiplus::PointF(cx, Config::CANVAS_HEIGHT - 25), &fmt, &tipBrush);

    m_backButton.Render(g, *m_buttonFont);
}

// ========== HUD ==========
void UIManager::RenderHUD(Gdiplus::Graphics& g, Player& player,
                          ScoreManager& score, const LevelManager& level) {
    wchar_t buf[128];
    // 分数
    swprintf(buf, 128, L"分数：%u    金币：%d", score.GetScore(), score.GetGold());
    Gdiplus::SolidBrush sb(Gdiplus::Color(255, 255, 240, 210));
    Gdiplus::StringFormat fmtL; fmtL.SetAlignment(Gdiplus::StringAlignmentNear);
    g.DrawString(buf, -1, m_hudFont, Gdiplus::PointF(10, 5), &fmtL, &sb);

    // 关卡
    Gdiplus::StringFormat fmtR; fmtR.SetAlignment(Gdiplus::StringAlignmentFar);
    const wchar_t* levelNames[5] = { L"地球轨道", L"小行星带", L"敌方舰队", L"深空要塞", L"母舰决战" };
    int lv = level.GetCurrentLevel() - 1;
    if (lv >= 0 && lv < 5) {
        swprintf(buf, 128, L"第%d关 · %ls", lv + 1, levelNames[lv]);
    } else {
        swprintf(buf, 128, L"第%d关", lv + 1);
    }
    Gdiplus::SolidBrush lb(Gdiplus::Color(200, 200, 200, 200));
    g.DrawString(buf, -1, m_hudFont, Gdiplus::PointF(Config::CANVAS_WIDTH - 10, 5), &fmtR, &lb);

    // 底部状态栏
    float yBase = Config::CANVAS_HEIGHT - 35.0f;
    Gdiplus::SolidBrush bgBar(Gdiplus::Color(140, 5, 10, 30));
    g.FillRectangle(&bgBar, 0.0f, yBase - 2.0f,
        static_cast<float>(Config::CANVAS_WIDTH), 38.0f);

    swprintf(buf, 128, L"生命：%d    炸弹：%d    武器：Lv%d",
             player.GetLives(), player.GetBombs(), player.GetWeaponLevel() + 1);
    Gdiplus::SolidBrush statBrush(Gdiplus::Color(220, 200, 240, 220));
    g.DrawString(buf, -1, m_hudFont, Gdiplus::PointF(10, yBase), &fmtL, &statBrush);

    if (player.HasSpeedBoost()) {
        Gdiplus::SolidBrush boostBrush(Gdiplus::Color(200, 80, 255, 80));
        g.DrawString(L"加速中", -1, m_smallFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH - 60, yBase), &fmtR, &boostBrush);
    }
}

// ========== 暂停/过渡/结束/排行榜 ==========
void UIManager::RenderPaused(Gdiplus::Graphics& g) {
    Gdiplus::SolidBrush overlay(Gdiplus::Color(150, 0, 0, 0));
    g.FillRectangle(&overlay, 0.0f, 0.0f,
        static_cast<float>(Config::CANVAS_WIDTH), static_cast<float>(Config::CANVAS_HEIGHT));

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    float cx = Config::CANVAS_WIDTH * 0.5f, cy = Config::CANVAS_HEIGHT * 0.5f;

    Gdiplus::SolidBrush pauseBrush(Gdiplus::Color(255, 255, 240, 200));
    g.DrawString(L"游 戏 暂 停", -1, m_menuFont, Gdiplus::PointF(cx, cy - 15), &fmt, &pauseBrush);
    g.DrawString(L"按 空格键 继续", -1, m_smallFont, Gdiplus::PointF(cx, cy + 25), &fmt, &pauseBrush);
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
    float cx = Config::CANVAS_WIDTH * 0.5f, cy = Config::CANVAS_HEIGHT * 0.5f;
    wchar_t buf[128];
    swprintf(buf, 128, L"第 %d 关 通关！", level.GetCurrentLevel() - 1);
    Gdiplus::SolidBrush tb(Gdiplus::Color(static_cast<BYTE>(255 * alpha), 255, 240, 80));
    g.DrawString(buf, -1, m_menuFont, Gdiplus::PointF(cx, cy), &fmt, &tb);
}

void UIManager::RenderGameOver(Gdiplus::Graphics& g, Player& /*player*/, ScoreManager& score) {
    Gdiplus::SolidBrush overlay(Gdiplus::Color(180, 15, 0, 5));
    g.FillRectangle(&overlay, 0.0f, 0.0f,
        static_cast<float>(Config::CANVAS_WIDTH), static_cast<float>(Config::CANVAS_HEIGHT));
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    float cx = Config::CANVAS_WIDTH * 0.5f;

    Gdiplus::SolidBrush overBrush(Gdiplus::Color(255, 255, 80, 60));
    g.DrawString(L"战 斗 结 束", -1, m_titleFont, Gdiplus::PointF(cx, 170), &fmt, &overBrush);

    wchar_t buf[64];
    swprintf(buf, 64, L"最终得分：%u    获得金币：%d", score.GetScore(), score.GetGold());
    Gdiplus::SolidBrush sb(Gdiplus::Color(220, 220, 220, 200));
    g.DrawString(buf, -1, m_smallFont, Gdiplus::PointF(cx, 250), &fmt, &sb);

    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(160, 160, 160, 160));
    g.DrawString(L"点击或按 Enter 返回大厅", -1, m_smallFont, Gdiplus::PointF(cx, 340), &fmt, &hintBrush);
}

// 商店逻辑
bool UIManager::HasShopItem(int id) const { return m_shopOwned[id]; }
int UIManager::GetShopItemCost(int id) const {
    int costs[8] = { 100, 200, 150, 50, 300, 250, 80, 120 };
    return costs[id];
}
void UIManager::BuyShopItem(int id, ScoreManager&) {
    m_shopOwned[id] = true;
}
