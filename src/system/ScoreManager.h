#pragma once

#include <string>

// ============================================================
// 分数管理器 — 分数追踪与最高分持久化
// ============================================================
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

    // 最高分
    void LoadHighScores();
    void SaveHighScores();
    bool IsHighScore() const;
    void InsertHighScore(const char* name);

    const HighScoreEntry* GetHighScores() const { return m_highScores; }
    int GetHighScoreCount() const { return 10; }

    void Reset();

private:
    unsigned int m_score = 0;
    int          m_combo = 0;
    HighScoreEntry m_highScores[10] = {};
};
