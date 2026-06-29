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

    void Reset();
    void ResetRun();

    // 统一存档：单文件原子写入金币+累计金币+商店已购+关卡进度
    // （修复旧版 SaveProgress 用 fopen("wb") 截断文件、把 SaveShopData 追加的
    //   SHOP 块冲掉的致命缺陷 —— 旧版 thunder.sav 仅 12 字节，商店能力从未落盘）
    void SaveAll(const bool* shopOwned, const bool* levelCleared,
                 const bool* levelUnlocked);
    void LoadAll(bool* shopOwned, bool* levelCleared, bool* levelUnlocked);

private:
    unsigned int m_score = 0;
    int          m_combo = 0;
    int          m_gold  = 0;
    int          m_totalGoldEarned = 0;
    HighScoreEntry m_highScores[10] = {};
};
