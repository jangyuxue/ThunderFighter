#pragma once

// ============================================================
// 雷霆战机 (Thunder Fighter) — 游戏配置与平衡常量
// ============================================================

namespace Config {

    // ---- 窗口 ----
    constexpr int CANVAS_WIDTH  = 480;
    constexpr int CANVAS_HEIGHT = 720;

    // ---- 游戏循环 ----
    constexpr double FIXED_TIMESTEP = 1.0 / 60.0;
    constexpr int    MAX_FRAMESKIP   = 5;

    // ---- 玩家 ----
    constexpr float PLAYER_SPEED_BALANCED = 5.0f;
    constexpr float PLAYER_SPEED_FAST     = 7.5f;
    constexpr float PLAYER_SPEED_POWER    = 4.0f;
    constexpr int   PLAYER_START_LIVES    = 3;
    constexpr int   PLAYER_MAX_LIVES      = 5;
    constexpr float PLAYER_INVINCIBLE_TIME = 1.5f;   // 受伤后无敌时间（秒）
    constexpr int   PLAYER_WIDTH  = 36;
    constexpr int   PLAYER_HEIGHT = 40;
    constexpr int   PLAYER_HITBOX_W = 20;   // 碰撞盒略小于视觉
    constexpr int   PLAYER_HITBOX_H = 24;

    // ---- 武器 ----
    constexpr float SHOOT_COOLDOWN_SINGLE = 0.15f;
    constexpr float SHOOT_COOLDOWN_DOUBLE = 0.13f;
    constexpr float SHOOT_COOLDOWN_SPREAD = 0.20f;
    constexpr float SHOOT_COOLDOWN_LASER  = 0.08f;
    constexpr int   MAX_WEAPON_LEVEL = 4;

    // ---- 子弹 ----
    constexpr int   MAX_BULLETS      = 300;
    constexpr float BULLET_SPEED     = 8.0f;
    constexpr float ENEMY_BULLET_SPEED = 4.0f;
    constexpr int   BULLET_WIDTH     = 4;
    constexpr int   BULLET_HEIGHT    = 12;
    constexpr int   LASER_WIDTH      = 6;

    // ---- 敌人 ----
    constexpr int   MAX_ENEMIES        = 60;
    constexpr float ENEMY_SMALL_SPEED  = 3.5f;
    constexpr float ENEMY_MEDIUM_SPEED = 2.0f;
    constexpr float BOSS_SPEED         = 1.0f;
    constexpr int   ENEMY_SMALL_HP     = 2;
    constexpr int   ENEMY_MEDIUM_HP    = 6;
    constexpr int   BOSS_BASE_HP       = 80;

    // ---- 道具 ----
    constexpr int   MAX_POWERUPS       = 10;
    constexpr float POWERUP_SPEED      = 2.0f;
    constexpr float POWERUP_DROP_CHANCE_SMALL  = 0.08f;
    constexpr float POWERUP_DROP_CHANCE_MEDIUM = 0.30f;
    constexpr float BOMB_DROP_CHANCE_MEDIUM    = 0.10f;
    constexpr float SPEED_BOOST_DURATION = 5.0f;
    constexpr float SPEED_BOOST_MULTIPLIER = 1.6f;

    // ---- 粒子 ----
    constexpr int   MAX_PARTICLES      = 500;
    constexpr float PARTICLE_LIFETIME  = 0.6f;

    // ---- 星空 ----
    constexpr int MAX_STARS = 120;

    // ---- 分数 ----
    constexpr int SCORE_SMALL_ENEMY  = 100;
    constexpr int SCORE_MEDIUM_ENEMY = 300;
    constexpr int SCORE_BOSS         = 5000;
    constexpr int SCORE_POWERUP      = 50;

    // ---- 关卡 ----
    constexpr int MAX_LEVELS = 5;
    constexpr float LEVEL_TRANSITION_TIME = 3.0f;

}  // namespace Config
