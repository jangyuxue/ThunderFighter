#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include "core/InputManager.h"
#include "entity/Player.h"
#include "system/ScoreManager.h"
#include "system/LevelManager.h"
#include "ui/Button.h"

// ============================================================
// UI 状态机 — 大厅模式
// ============================================================
enum class GameState {
    HUB,              // 大厅主界面
    PLAYING,
    PAUSED,
    LEVEL_SELECT,     // 关卡选择
    SHOP,             // 商店
    MISSIONS,         // 任务
    ACHIEVEMENTS,     // 成就
    LEVEL_TRANSITION,
    GAME_OVER,
    HIGH_SCORE
};

class UIManager {
public:
    UIManager();
    ~UIManager();

    void Init();
    void LoadData(ScoreManager& score);   // 读档
    void SaveData(ScoreManager& score);   // 存档
    void OnLevelCleared(int level);        // 关卡通关回调
    void SetState(GameState state);
    GameState GetState() const { return m_state; }

    void HandleInput(InputManager& input, Player& player, ScoreManager& score);
    void Update(float dt);
    void Render(Gdiplus::Graphics& g,
                Player& player,
                ScoreManager& score,
                const LevelManager& level);

    int  GetSelectedAircraft() const { return m_selectedAircraft; }
    int  GetSelectedLevel()    const { return m_selectedLevel; }
    bool IsGameStarted() const { return m_gameStarted; }
    bool ShouldStartGame() const { return m_shouldStartGame; }
    void ResetStartFlag() { m_shouldStartGame = false; }

    // 商店/任务/成就相关
    bool HasShopItem(int id) const;
    void BuyShopItem(int id, ScoreManager& score);
    int  GetShopItemCost(int id) const;

private:
    // 渲染子界面
    void RenderHub(Gdiplus::Graphics& g, ScoreManager& score);
    void RenderHUD(Gdiplus::Graphics& g, Player& player,
                   ScoreManager& score, const LevelManager& level);
    void RenderPaused(Gdiplus::Graphics& g);
    void RenderLevelSelect(Gdiplus::Graphics& g, ScoreManager& score);
    void RenderShop(Gdiplus::Graphics& g, ScoreManager& score);
    void RenderMissions(Gdiplus::Graphics& g, ScoreManager& score);
    void RenderAchievements(Gdiplus::Graphics& g);
    void RenderLevelTransition(Gdiplus::Graphics& g, const LevelManager& level);
    void RenderGameOver(Gdiplus::Graphics& g, Player& player, ScoreManager& score);
    void RenderHighScore(Gdiplus::Graphics& g, ScoreManager& score);

    // 按钮管理
    void UpdateHubButtons(int mx, int my, bool down, bool pressed);
    void UpdateLevelSelectButtons(int mx, int my, bool down, bool pressed);
    void UpdateShopButtons(int mx, int my, bool down, bool pressed, ScoreManager& score);
    void UpdateMissionsButtons(int mx, int my, bool down, bool pressed, ScoreManager& score);
    void UpdateAchievementsButtons(int mx, int my, bool down, bool pressed);

    GameState m_state = GameState::HUB;
    GameState m_prevState = GameState::HUB;

    int  m_selectedAircraft = 1;   // 0=Speed, 1=Balanced, 2=Power
    int  m_selectedLevel    = 1;
    bool m_gameStarted      = false;
    bool m_shouldStartGame  = false;

    float m_menuTimer     = 0.0f;
    float m_transitionAlpha = 0.0f;
    float m_backTimer     = 0.0f;   // ESC 返回冷却

    // HUB 按钮
    std::vector<Button> m_hubButtons;       // 6个主按钮
    Button m_aircraftLeft, m_aircraftRight;  // 战机选择箭头
    std::vector<Button> m_levelButtons;      // 关卡选择按钮
    std::vector<Button> m_shopButtons;       // 商店按钮
    std::vector<Button> m_missionButtons;    // 任务按钮
    std::vector<Button> m_achievementButtons;// 成就按钮
    Button m_backButton;

    // 商店已购标记
    bool m_shopOwned[8] = {};

    // 任务进度
    int  m_missionProgress[8] = {};
    bool m_missionClaimed[8] = {};

    // 成就状态
    bool m_achievementUnlocked[12] = {};
    float m_achievementNotifyTimer = 0.0f;
    int   m_achievementNotifyId = -1;

    // 关卡解锁
    bool m_levelUnlocked[5] = { true, false, false, false, false };
    bool m_levelCleared[5] = {};

    // 字体
    Gdiplus::Font* m_titleFont  = nullptr;
    Gdiplus::Font* m_menuFont   = nullptr;
    Gdiplus::Font* m_buttonFont = nullptr;
    Gdiplus::Font* m_smallFont  = nullptr;
    Gdiplus::Font* m_hudFont    = nullptr;
    std::wstring m_fontName;   // 系统中文字体名，供内联 Font 复用（确保 CJK 字形）
};
