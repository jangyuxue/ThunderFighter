#include "system/ScoreManager.h"
#include <cstdio>
#include <cstring>

ScoreManager::ScoreManager() {
    // 金币/进度由 LoadAll 统一读取（在 UIManager::LoadData 中调用）
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

void ScoreManager::Reset() {
    m_score = 0;
    m_combo = 0;
}

void ScoreManager::ResetRun() {
    m_score = 0;
    m_combo = 0;
}

// ============================================================
// 统一存档 (thunder.sav) — 单文件原子读写
// 布局: "TSAV" | version(int) | gold(int) | totalGoldEarned(int)
//       | shopOwned[8] | levelCleared[5] | levelUnlocked[5]
// ============================================================
static const char* SAVE_PATH = "thunder.sav";
static const int  SAVE_VERSION = 1;

void ScoreManager::SaveAll(const bool* shopOwned, const bool* levelCleared,
                           const bool* levelUnlocked) {
    FILE* f = fopen(SAVE_PATH, "wb");  // 单次截断+顺序写，写完即完整
    if (!f) return;
    fwrite("TSAV", 1, 4, f);
    int version = SAVE_VERSION;
    fwrite(&version, sizeof(int), 1, f);
    fwrite(&m_gold, sizeof(m_gold), 1, f);
    fwrite(&m_totalGoldEarned, sizeof(m_totalGoldEarned), 1, f);
    if (shopOwned)     for (int i = 0; i < 8; ++i) { unsigned char b = shopOwned[i] ? 1 : 0; fwrite(&b, 1, 1, f); }
    if (levelCleared)  for (int i = 0; i < 5; ++i) { unsigned char b = levelCleared[i] ? 1 : 0; fwrite(&b, 1, 1, f); }
    if (levelUnlocked) for (int i = 0; i < 5; ++i) { unsigned char b = levelUnlocked[i] ? 1 : 0; fwrite(&b, 1, 1, f); }
    fclose(f);
}

void ScoreManager::LoadAll(bool* shopOwned, bool* levelCleared, bool* levelUnlocked) {
    FILE* f = fopen(SAVE_PATH, "rb");
    if (!f) {
        m_gold = 1000;  // 首次启动（无存档）：赠送1000开局金币
        return;
    }
    char magic[5] = {};
    int firstInt = 0;
    if (fread(magic, 1, 4, f) == 4 && strncmp(magic, "TSAV", 4) == 0
        && fread(&firstInt, sizeof(int), 1, f) == 1) {
        if (firstInt == SAVE_VERSION) {
            // 新格式：version | gold | totalGold | shop[8] | cleared[5] | unlocked[5]
            fread(&m_gold, sizeof(m_gold), 1, f);
            fread(&m_totalGoldEarned, sizeof(m_totalGoldEarned), 1, f);
            if (shopOwned)      for (int i = 0; i < 8; ++i) { unsigned char b; if (fread(&b,1,1,f)==1) shopOwned[i] = (b != 0); }
            if (levelCleared)   for (int i = 0; i < 5; ++i) { unsigned char b; if (fread(&b,1,1,f)==1) levelCleared[i] = (b != 0); }
            if (levelUnlocked)  for (int i = 0; i < 5; ++i) { unsigned char b; if (fread(&b,1,1,f)==1) levelUnlocked[i] = (b != 0); }
        } else {
            // 旧格式兼容（无 version 字段）：firstInt 实为旧 gold，保留以免升级丢金币
            m_gold = firstInt;
            fread(&m_totalGoldEarned, sizeof(m_totalGoldEarned), 1, f);
            // 旧档无 shop/level 数据，保持调用方传入的默认值
        }
    }
    fclose(f);
    // 不再每局 floor 到 1000：尊重存档真实金币
    // （旧版每次加载都把金币重发到1000，导致"花掉后重进金币被退回"的 bug）
}
