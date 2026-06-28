#include "ui/UIManager.h"
#include "config/GameConfig.h"

UIManager::UIManager() = default;

void UIManager::SetState(GameState state) {
    m_state = state;
    m_transitionAlpha = 0.0f;
    if (state == GameState::LEVEL_TRANSITION) {
        m_transitionAlpha = 1.0f;
    }
}

void UIManager::HandleInput(InputManager& input, Player& player) {
    switch (m_state) {
    case GameState::MENU:
        if (input.IsKeyPressed(VK_UP) || input.IsKeyPressed('W')) {
            m_selectedAircraft = (m_selectedAircraft + 2) % 3;
        }
        if (input.IsKeyPressed(VK_DOWN) || input.IsKeyPressed('S')) {
            m_selectedAircraft = (m_selectedAircraft + 1) % 3;
        }
        if (input.IsConfirmPressed()) {
            m_gameStarted = true;
            SetState(GameState::PLAYING);
            player.Init(static_cast<PlayerType>(m_selectedAircraft));
        }
        if (input.IsKeyPressed('H')) {
            SetState(GameState::HIGH_SCORE);
        }
        break;

    case GameState::PLAYING:
        if (input.IsPausePressed()) {
            SetState(GameState::PAUSED);
        }
        break;

    case GameState::PAUSED:
        if (input.IsPausePressed() || input.IsBackPressed()) {
            SetState(GameState::PLAYING);
        }
        break;

    case GameState::GAME_OVER:
        if (input.IsConfirmPressed()) {
            m_gameStarted = true;
            SetState(GameState::MENU);
        }
        break;

    case GameState::HIGH_SCORE:
        if (input.IsBackPressed()) {
            SetState(GameState::MENU);
        }
        break;

    case GameState::LEVEL_TRANSITION:
        break;
    }
}

void UIManager::Update(float dt) {
    m_menuTimer += dt;
    if (m_damageFlashTimer > 0.0f) {
        m_damageFlashTimer -= dt;
    }

    if (m_state == GameState::LEVEL_TRANSITION) {
        m_transitionAlpha -= dt * 0.5f;
        if (m_transitionAlpha < 0.0f) m_transitionAlpha = 0.0f;
    }
}

void UIManager::Render(Gdiplus::Graphics& g,
                       Player& player,
                       ScoreManager& score,
                       LevelManager& level) {
    switch (m_state) {
    case GameState::MENU:
        RenderMenu(g);
        break;
    case GameState::PLAYING:
        RenderHUD(g, player, score, level);
        break;
    case GameState::PAUSED:
        RenderHUD(g, player, score, level);
        RenderPaused(g);
        break;
    case GameState::LEVEL_TRANSITION:
        RenderHUD(g, player, score, level);
        RenderLevelTransition(g, level);
        break;
    case GameState::GAME_OVER:
        RenderGameOver(g, player, score);
        break;
    case GameState::HIGH_SCORE:
        RenderHighScore(g, score);
        break;
    }
}

void UIManager::RenderMenu(Gdiplus::Graphics& g) {
    // 半透明背景
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(200, 5, 5, 18));
    g.FillRectangle(&bgBrush, 0, 0, Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT);

    Gdiplus::Font titleFont(L"Arial", 36, Gdiplus::FontStyleBold);
    Gdiplus::Font menuFont(L"Arial", 18, Gdiplus::FontStyleRegular);
    Gdiplus::Font smallFont(L"Arial", 13, Gdiplus::FontStyleRegular);

    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    // 游戏标题
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"THUNDER FIGHTER", -1, &titleFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 100), &fmt, &titleBrush);

    Gdiplus::SolidBrush subBrush(Gdiplus::Color(200, 200, 200, 200));
    g.DrawString(L"雷霆战机", -1, &menuFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 145), &fmt, &subBrush);

    // 选择战机
    float selectY = 230.0f;
    Gdiplus::SolidBrush selTitleBrush(Gdiplus::Color(200, 180, 180, 180));
    g.DrawString(L"SELECT AIRCRAFT", -1, &smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, selectY - 30),
                 &fmt, &selTitleBrush);

    const wchar_t* names[] = { L"⚡ LIGHTNING  (SPEED)", L"🌪 HURRICANE   (BALANCED)", L"🔥 INFERNO     (POWER)" };
    Gdiplus::Color colors[] = {
        Gdiplus::Color(255, 80, 160, 255),
        Gdiplus::Color(255, 60, 220, 80),
        Gdiplus::Color(255, 255, 60, 60)
    };
    const wchar_t* descs[] = {
        L"Fast & Agile  |  Single Shot  |  2 HP",
        L"Balanced      |  Standard     |  3 HP",
        L"Heavy Fire    |  Double Shot  |  4 HP"
    };

    for (int i = 0; i < 3; ++i) {
        float y = selectY + i * 60.0f;
        bool sel = (i == m_selectedAircraft);

        Gdiplus::SolidBrush nameBrush(colors[i]);
        Gdiplus::Font nameFont(L"Arial", sel ? 20.0f : 16.0f,
                               sel ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular);
        g.DrawString(names[i], -1, &nameFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, y),
                     &fmt, &nameBrush);

        Gdiplus::SolidBrush descBrush(Gdiplus::Color(150, 150, 150, 150));
        g.DrawString(descs[i], -1, &smallFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, y + 22),
                     &fmt, &descBrush);

        if (sel) {
            Gdiplus::Pen selPen(Gdiplus::Color(255, 255, 255, 200), 1.5f);
            g.DrawRectangle(&selPen, Config::CANVAS_WIDTH * 0.5f - 120.0f,
                            y - 14.0f, 240.0f, 42.0f);
        }
    }

    // 操作提示
    float hintY = 500.0f;
    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(180, 160, 160, 160));
    Gdiplus::Font hintFont(L"Arial", 11);

    g.DrawString(L"ARROWS / WASD — Move    J — Shoot    K — Bomb    ESC — Pause",
                 -1, &hintFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, hintY),
                 &fmt, &hintBrush);

    g.DrawString(L"ENTER — Start    H — High Scores",
                 -1, &hintFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, hintY + 22),
                 &fmt, &hintBrush);

    // 闪烁提示
    float blink = 0.5f + 0.5f * sin(m_menuTimer * 3.0f);
    Gdiplus::Color pressColor(static_cast<BYTE>(200 * blink), 255, 255, 100);
    Gdiplus::SolidBrush pressBrush(pressColor);
    g.DrawString(L"PRESS ENTER TO START", -1, &menuFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, hintY + 60),
                 &fmt, &pressBrush);
}

void UIManager::RenderHUD(Gdiplus::Graphics& g,
                          Player& player,
                          ScoreManager& score,
                          LevelManager& level) {
    Gdiplus::Font hudFont(L"Consolas", 14, Gdiplus::FontStyleBold);
    Gdiplus::Font smallFont(L"Consolas", 11);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentNear);

    // 分数
    wchar_t buf[64];
    swprintf(buf, 64, L"SCORE: %u", score.GetScore());
    Gdiplus::SolidBrush scoreBrush(Gdiplus::Color(255, 255, 255, 200));
    g.DrawString(buf, -1, &hudFont, Gdiplus::PointF(10, 5), &fmt, &scoreBrush);

    // Combo
    if (score.GetCombo() > 1) {
        swprintf(buf, 64, L"COMBO x%d", score.GetCombo());
        Gdiplus::SolidBrush comboBrush(Gdiplus::Color(255, 255, 200, 80));
        g.DrawString(buf, -1, &smallFont, Gdiplus::PointF(10, 25), &fmt, &comboBrush);
    }

    // 关卡
    swprintf(buf, 64, L"LEVEL %d - %s", level.GetCurrentLevel(),
             level.GetLevelName().c_str());
    Gdiplus::SolidBrush levelBrush(Gdiplus::Color(200, 200, 200, 200));
    Gdiplus::StringFormat fmtR;
    fmtR.SetAlignment(Gdiplus::StringAlignmentFar);
    g.DrawString(buf, -1, &smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH - 10, 5), &fmtR, &levelBrush);

    // 生命数
    float lifeY = Config::CANVAS_HEIGHT - 30.0f;
    Gdiplus::SolidBrush lifeBrush(Gdiplus::Color(255, 80, 220, 80));
    swprintf(buf, 64, L"LIVES: %d", player.GetLives());
    g.DrawString(buf, -1, &hudFont, Gdiplus::PointF(10, lifeY), &fmt, &lifeBrush);

    // 炸弹数
    Gdiplus::SolidBrush bombBrush(Gdiplus::Color(255, 255, 100, 80));
    swprintf(buf, 64, L"BOMBS: %d", player.GetBombs());
    g.DrawString(buf, -1, &smallFont, Gdiplus::PointF(10, lifeY + 20),
                 &fmt, &bombBrush);

    // 武器等级
    const wchar_t* wepNames[] = { L"LV1:SINGLE", L"LV2:DOUBLE", L"LV3:TRIPLE", L"LV4:SPREAD", L"LV5:LASER" };
    int wl = player.GetWeaponLevel();
    if (wl >= 0 && wl <= 4) {
        Gdiplus::SolidBrush wepBrush(Gdiplus::Color(200, 255, 240, 100));
        g.DrawString(wepNames[wl], -1, &smallFont,
                     Gdiplus::PointF(10, lifeY + 38), &fmt, &wepBrush);
    }

    // 护盾指示
    if (player.GetShields() > 0) {
        swprintf(buf, 64, L"SHIELD: %d", player.GetShields());
        Gdiplus::SolidBrush shieldBrush(Gdiplus::Color(200, 100, 200, 255));
        g.DrawString(buf, -1, &smallFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH - 10, lifeY + 38),
                     &fmtR, &shieldBrush);
    }

    // 加速计时器
    if (player.HasSpeedBoost()) {
        Gdiplus::SolidBrush boostBrush(Gdiplus::Color(200, 80, 255, 80));
        g.DrawString(L"BOOST!", -1, &smallFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH - 10, lifeY + 20),
                     &fmtR, &boostBrush);
    }

    // 无敌闪烁提示
    if (player.IsInvincible()) {
        Gdiplus::SolidBrush invBrush(Gdiplus::Color(180, 255, 255, 100));
        g.DrawString(L"INVINCIBLE", -1, &smallFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, Config::CANVAS_HEIGHT - 30),
                     &fmtR, &invBrush);
    }
}

void UIManager::RenderPaused(Gdiplus::Graphics& g) {
    // 半透明覆盖
    Gdiplus::SolidBrush overlay(Gdiplus::Color(150, 0, 0, 0));
    g.FillRectangle(&overlay, 0, 0, Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT);

    Gdiplus::Font font(L"Arial", 36, Gdiplus::FontStyleBold);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush pausedBrush(Gdiplus::Color(255, 255, 255, 200));
    g.DrawString(L"PAUSED", -1, &font,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f,
                                 Config::CANVAS_HEIGHT * 0.5f - 20),
                 &fmt, &pausedBrush);

    Gdiplus::Font smallFont(L"Arial", 14);
    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(200, 200, 200, 200));
    g.DrawString(L"Press SPACE to continue", -1, &smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f,
                                 Config::CANVAS_HEIGHT * 0.5f + 30),
                 &fmt, &hintBrush);
}

void UIManager::RenderLevelTransition(Gdiplus::Graphics& g,
                                       LevelManager& level) {
    float alpha = m_transitionAlpha;
    if (alpha <= 0.0f) return;

    Gdiplus::SolidBrush overlay(Gdiplus::Color(static_cast<BYTE>(200 * alpha),
                                                0, 0, 0));
    g.FillRectangle(&overlay, 0, 0, Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT);

    Gdiplus::Font font(L"Arial", 28, Gdiplus::FontStyleBold);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    wchar_t buf[128];
    swprintf(buf, 128, L"LEVEL %d COMPLETE!", level.GetCurrentLevel() - 1);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(static_cast<BYTE>(255 * alpha),
                                                  255, 255, 100));
    g.DrawString(buf, -1, &font,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f,
                                 Config::CANVAS_HEIGHT * 0.5f - 25),
                 &fmt, &textBrush);

    Gdiplus::Font smallFont(L"Arial", 16);
    swprintf(buf, 128, L"Entering: %s", level.GetLevelName().c_str());
    g.DrawString(buf, -1, &smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f,
                                 Config::CANVAS_HEIGHT * 0.5f + 25),
                 &fmt, &textBrush);
}

void UIManager::RenderGameOver(Gdiplus::Graphics& g,
                                Player& /*player*/,
                                ScoreManager& score) {
    // 半透明覆盖
    Gdiplus::SolidBrush overlay(Gdiplus::Color(180, 10, 0, 0));
    g.FillRectangle(&overlay, 0, 0, Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT);

    Gdiplus::Font bigFont(L"Arial", 40, Gdiplus::FontStyleBold);
    Gdiplus::Font midFont(L"Arial", 22);
    Gdiplus::Font smallFont(L"Arial", 14);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush overBrush(Gdiplus::Color(255, 255, 60, 60));
    g.DrawString(L"GAME OVER", -1, &bigFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 200),
                 &fmt, &overBrush);

    wchar_t buf[64];
    swprintf(buf, 64, L"FINAL SCORE: %u", score.GetScore());
    Gdiplus::SolidBrush scoreBrush(Gdiplus::Color(255, 255, 255, 200));
    g.DrawString(buf, -1, &midFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 270),
                 &fmt, &scoreBrush);

    if (score.IsHighScore()) {
        Gdiplus::SolidBrush hsBrush(Gdiplus::Color(255, 255, 220, 60));
        g.DrawString(L"★ NEW HIGH SCORE! ★", -1, &midFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 310),
                     &fmt, &hsBrush);
    }

    Gdiplus::SolidBrush hintBrush(Gdiplus::Color(200, 180, 180, 180));
    g.DrawString(L"Press ENTER to return to menu", -1, &smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 400),
                 &fmt, &hintBrush);
}

void UIManager::RenderHighScore(Gdiplus::Graphics& g, ScoreManager& score) {
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(200, 5, 5, 18));
    g.FillRectangle(&bgBrush, 0, 0, Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT);

    Gdiplus::Font titleFont(L"Arial", 28, Gdiplus::FontStyleBold);
    Gdiplus::Font entryFont(L"Arial", 15);
    Gdiplus::Font smallFont(L"Arial", 12);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 255, 220, 80));
    g.DrawString(L"HIGH SCORES", -1, &titleFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 50), &fmt, &titleBrush);

    wchar_t buf[128];
    for (int i = 0; i < 10; ++i) {
        float y = 110.0f + i * 35.0f;
        const auto& entry = score.GetHighScores()[i];

        if (entry.score > 0) {
            swprintf(buf, 128, L"%2d.  %-12s  %u",
                     i + 1, entry.name, entry.score);
        } else {
            swprintf(buf, 128, L"%2d.  ---", i + 1);
        }

        Gdiplus::SolidBrush entryBrush(Gdiplus::Color(200, 200, 200, 200));
        g.DrawString(buf, -1, &entryFont,
                     Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, y),
                     &fmt, &entryBrush);
    }

    Gdiplus::SolidBrush backBrush(Gdiplus::Color(180, 160, 160, 160));
    g.DrawString(L"Press ESC to return", -1, &smallFont,
                 Gdiplus::PointF(Config::CANVAS_WIDTH * 0.5f, 500),
                 &fmt, &backBrush);
}
