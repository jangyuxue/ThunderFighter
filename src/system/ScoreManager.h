#pragma once

#include <string>

struct HighScoreEntry {
    unsigned int score;
    char name[32];
};

class ScoreManager {
public:
    ScoreManager();

    void AddScore(unsigned int points);
    unsigned int GetScore() const { return m_score; }

    void AddCombo();
    void ResetCombo();
    int  GetCombo() const { return m_combo; }

    // 金币系统
    void AddGold(int amount) { m_gold += amount; }
    bool SpendGold(int amount);
    int  GetGold() const { return m_gold; }
    int  GetTotalGoldEarned() const { return m_totalGoldEarned; }

    // 最高分
    void LoadHighScores();
    void SaveHighScores();
    bool IsHighScore() const;
    void InsertHighScore(const char* name);

    const HighScoreEntry* GetHighScores() const { return m_highScores; }
    int GetHighScoreCount() const { return 10; }

    void SaveProgress();   // 保存金币等
    void LoadProgress();   // 读取进度
    void SaveShopData(const bool* owned, int count);   // 保存商店购买
    void LoadShopData(bool* owned, int count);         // 读取商店购买
    void Reset();
    void ResetRun();

private:
    unsigned int m_score = 0;
    int          m_combo = 0;
    int          m_gold  = 0;
    int          m_totalGoldEarned = 0;
    HighScoreEntry m_highScores[10] = {};
};
