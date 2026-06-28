#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include "core/InputManager.h"
#include "entity/Player.h"
#include "system/ScoreManager.h"
#include "system/LevelManager.h"

// ============================================================
// UI 管理器 — 状态机驱动
// ============================================================
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    LEVEL_TRANSITION,
    GAME_OVER,
    HIGH_SCORE
};

class UIManager {
public:
    UIManager();

    void SetState(GameState state);
    GameState GetState() const { return m_state; }

    void HandleInput(InputManager& input, Player& player);
    void Update(float dt);
    void Render(Gdiplus::Graphics& g,
                Player& player,
                ScoreManager& score,
                LevelManager& level);

    // 菜单选择
    int  GetSelectedAircraft() const { return m_selectedAircraft; }
    bool IsGameStarted() const { return m_gameStarted; }

private:
    void RenderMenu(Gdiplus::Graphics& g);
    void RenderHUD(Gdiplus::Graphics& g,
                   Player& player,
                   ScoreManager& score,
                   LevelManager& level);
    void RenderPaused(Gdiplus::Graphics& g);
    void RenderLevelTransition(Gdiplus::Graphics& g, LevelManager& level);
    void RenderGameOver(Gdiplus::Graphics& g,
                        Player& player,
                        ScoreManager& score);
    void RenderHighScore(Gdiplus::Graphics& g, ScoreManager& score);

    GameState m_state         = GameState::MENU;
    int       m_selectedAircraft = 1;   // 0=Speed, 1=Balanced, 2=Power
    bool      m_gameStarted   = false;
    float     m_transitionAlpha = 0.0f;

    // 菜单动画
    float     m_menuTimer     = 0.0f;
    int       m_menuBlink     = 0;

    // 闪烁计时器（受击后 HUD 红闪）
    float     m_damageFlashTimer = 0.0f;
};
