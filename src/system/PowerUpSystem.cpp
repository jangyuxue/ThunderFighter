#include "system/PowerUpSystem.h"

PowerUpSystem::PowerUpSystem()
    : m_rng(std::random_device{}()) {
}

void PowerUpSystem::TryDrop(float x, float y, bool isBoss,
                            std::vector<std::unique_ptr<PowerUp>>& powerUps) {
    if (isBoss) {
        SpawnBossDrops(x, y, powerUps);
        return;
    }

    // 随机掉落（30% 概率）
    float roll = static_cast<float>(m_rng() % 100) / 100.0f;
    if (roll < Config::POWERUP_DROP_CHANCE_MEDIUM) {
        auto p = std::make_unique<PowerUp>();
        p->Init(x, y, PowerUp::RandomType());
        powerUps.push_back(std::move(p));
    }
}

void PowerUpSystem::SpawnBossDrops(float x, float y,
                                   std::vector<std::unique_ptr<PowerUp>>& powerUps) {
    // Boss 击败后必定掉落武器升级 + 炸弹
    auto pw = std::make_unique<PowerUp>();
    pw->Init(x - 30, y, PowerUpType::WEAPON_UP);
    powerUps.push_back(std::move(pw));

    auto pb = std::make_unique<PowerUp>();
    pb->Init(x + 30, y, PowerUpType::BOMB);
    powerUps.push_back(std::move(pb));

    // 额外随机道具
    for (int i = 0; i < 2; ++i) {
        auto p = std::make_unique<PowerUp>();
        p->Init(x + static_cast<float>(rand() % 80 - 40),
                y + static_cast<float>(rand() % 30 - 15),
                PowerUp::RandomType());
        powerUps.push_back(std::move(p));
    }
}

void PowerUpSystem::Apply(Player& player, PowerUpType type) {
    switch (type) {
    case PowerUpType::WEAPON_UP:
        player.UpgradeWeapon();
        break;
    case PowerUpType::SHIELD:
        player.AddShield();
        break;
    case PowerUpType::SPEED_BOOST:
        player.ActivateSpeedBoost();
        break;
    case PowerUpType::BOMB:
        player.AddBomb();
        break;
    }
}
