#include "system/ScoreManager.h"
#include <cstdio>
#include <cstring>

ScoreManager::ScoreManager() {
    memset(m_highScores, 0, sizeof(m_highScores));
    LoadHighScores();
    LoadProgress();
    if (m_gold < 1000) m_gold = 1000;  // 开局至少1000金币
}

void ScoreManager::AddScore(unsigned int points) {
    m_score += points * (1 + m_combo / 10);
}

void ScoreManager::AddCombo() { ++m_combo; }
void ScoreManager::ResetCombo() { m_combo = 0; }

bool ScoreManager::SpendGold(int amount) {
    if (m_gold < amount) return false;
    m_gold -= amount;
    return true;
}

bool ScoreManager::IsHighScore() const {
    for (int i = 0; i < 10; ++i)
        if (m_score > m_highScores[i].score) return true;
    return false;
}

void ScoreManager::InsertHighScore(const char* name) {
    int insertAt = 10;
    for (int i = 0; i < 10; ++i) {
        if (m_score > m_highScores[i].score) { insertAt = i; break; }
    }
    if (insertAt >= 10) return;
    for (int i = 9; i > insertAt; --i) m_highScores[i] = m_highScores[i - 1];
    m_highScores[insertAt].score = m_score;
    strncpy(m_highScores[insertAt].name, name, 31);
    m_highScores[insertAt].name[31] = '\0';
    SaveHighScores();
}

void ScoreManager::LoadHighScores() {
    FILE* f = fopen("thunder.hsc", "rb");
    if (!f) return;
    char magic[5] = {};
    if (fread(magic, 1, 4, f) == 4 && strncmp(magic, "THDR", 4) == 0)
        fread(m_highScores, sizeof(HighScoreEntry), 10, f);
    fclose(f);
}

void ScoreManager::SaveHighScores() {
    FILE* f = fopen("thunder.hsc", "wb");
    if (!f) return;
    fwrite("THDR", 1, 4, f);
    fwrite(m_highScores, sizeof(HighScoreEntry), 10, f);
    fclose(f);
}

void ScoreManager::Reset() {
    m_score = 0;
    m_combo = 0;
}

void ScoreManager::ResetRun() {
    m_score = 0;
    m_combo = 0;
}

void ScoreManager::SaveProgress() {
    FILE* f = fopen("thunder.sav", "wb");
    if (!f) return;
    fwrite("TSAV", 1, 4, f);
    fwrite(&m_gold, sizeof(m_gold), 1, f);
    fwrite(&m_totalGoldEarned, sizeof(m_totalGoldEarned), 1, f);
    fclose(f);
}

void ScoreManager::LoadProgress() {
    FILE* f = fopen("thunder.sav", "rb");
    if (!f) return;
    char magic[5] = {};
    if (fread(magic, 1, 4, f) == 4 && strncmp(magic, "TSAV", 4) == 0) {
        fread(&m_gold, sizeof(m_gold), 1, f);
        fread(&m_totalGoldEarned, sizeof(m_totalGoldEarned), 1, f);
    }
    fclose(f);
}
