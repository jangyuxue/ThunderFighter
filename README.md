# Thunder Fighter (雷霆战机)

C++ 课程大作业 — 自上而下卷轴射击游戏

![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20GDI%2B-green)
![Lines](https://img.shields.io/badge/LoC-3216-orange)
![Build](https://img.shields.io/badge/Build-Zero%20Warnings-success)

## 项目简介

雷霆战机是一款经典的自上而下卷轴射击游戏，玩家驾驶战机在太空中与敌机作战，通过五关挑战最终击败母舰 Boss。

- **开发语言**: C++17
- **图形引擎**: Windows GDI+（零外部依赖）
- **编译器**: g++ (MinGW-w64)
- **代码规模**: 42 个源文件，3,216 行代码

## 快速开始

### 编译

```bash
g++ -std=c++17 -Wall -Wextra -O2 -I src \
    src/*.cpp src/core/*.cpp src/entity/*.cpp \
    src/system/*.cpp src/ui/*.cpp \
    -o ThunderFighter.exe \
    -lgdiplus -lgdi32 -luser32 -lkernel32 -mwindows
```

或使用 Makefile：

```bash
make        # 编译
make run    # 编译并运行
make clean  # 清理
```

### 运行

直接双击 `ThunderFighter.exe`，或在命令行执行：

```bash
./ThunderFighter.exe
```

无需安装任何外部库或运行时。在任何 Windows 系统上可直接运行。

## 操作说明

| 按键 | 功能 |
|------|------|
| ↑↓←→ / WASD | 移动飞机 |
| **J** | 射击 |
| **K** | 释放炸弹（清屏） |
| **Enter** | 确认 / 开始游戏 |
| **Space** | 暂停 / 继续 |
| **ESC** | 返回菜单 |

## 游戏内容

### 三种战机

| 型号 | 速度 | 火力 | 生命 | 颜色 | 风格 |
|------|------|------|------|------|------|
| ⚡ 闪电 Lightning | 7.5 | 单发 | 2 | 蓝 | 灵活闪避 |
| 🌪 飓风 Hurricane | 5.0 | 标准 | 3 | 绿 | 全能均衡 |
| 🔥 烈焰 Inferno | 4.0 | 双发 | 4 | 红 | 重火力坦克 |

### 五关闯关

| 关卡 | 名称 | 敌人特点 | Boss HP |
|------|------|----------|---------|
| 1 | 地球轨道 Earth Orbit | 小型敌机为主 | 60 |
| 2 | 小行星带 Asteroid Belt | 中型敌机登场 | 100 |
| 3 | 敌方舰队 Enemy Fleet | 密集混合编队 | 150 |
| 4 | 深空要塞 Deep Space Fortress | 高速连续波次 | 200 |
| 5 | 母舰决战 Mothership | 终极决战 | 300 |

### 武器升级链

```
LV1 单发 → LV2 双发 → LV3 三发 → LV4 散射 → LV5 激光
```

### 道具系统

| 道具 | 标记 | 效果 | 掉落概率 |
|------|------|------|----------|
| 武器升级 Weapon Up | W | 武器等级+1 | 30%（中型）/ 100%（Boss） |
| 护盾 Shield | S | 抵挡一次伤害 | 25% |
| 加速 Boost | B | 5秒移速提升 | 20% |
| 炸弹 Bomb | X | 炸弹数+1 | 15% |

### Boss 战特点

- **三阶段弹幕变化**：瞄准射击 → 扇形弹幕 → 螺旋弹幕
- 每个 Boss 有独立血条显示
- Boss 击败后掉落大量道具
- 多阶段爆炸特效

## 软件架构

```
src/
├── main.cpp              # WinMain 程序入口
├── Game.h/cpp            # 游戏核心（主循环、碰撞、渲染协调）
├── core/                 # 引擎层
│   ├── Window            # Win32 窗口封装
│   ├── GameTimer         # QPC 高精度计时器
│   ├── InputManager      # 键盘输入管理（GetAsyncKeyState）
│   ├── Renderer          # GDI+ 双缓冲渲染器
│   ├── ResourceManager   # GDI+ 资源缓存（Pen/Brush/Font）
│   ├── ObjectPool        # 模板对象池（子弹/粒子复用）
│   └── Renderable        # 可渲染对象接口
├── entity/               # 实体层
│   ├── Entity            # 实体基类（位置、速度、AABB碰撞）
│   ├── Player            # 玩家飞机（3种机型）
│   ├── Bullet            # 子弹（普通/激光）
│   ├── Enemy             # 敌人（Small/Medium/Boss）
│   ├── PowerUp           # 道具（4种类型）
│   └── Particle          # 粒子特效（爆炸/火花/拖尾）
├── system/               # 系统层
│   ├── StarField         # 视差星空背景（3层）
│   ├── LevelManager      # 关卡/波次管理
│   ├── WeaponSystem      # 武器系统（5级射击模式）
│   ├── PowerUpSystem     # 道具掉落与效果
│   ├── ParticleSystem    # 粒子特效管理
│   └── ScoreManager      # 分数与最高分管理
├── ui/                   # 界面层
│   └── UIManager         # UI状态机（菜单/游戏/暂停/结束）
└── config/               # 配置
    ├── GameConfig.h      # 游戏平衡常量
    └── LevelData.h       # 关卡数据定义
```

### 模块交互

```
Game (总控)
 ├── Renderer (GDI+ 双缓冲)
 ├── InputManager (键盘轮询)
 ├── GameTimer (QPC 计时)
 ├── StarField (背景星空)
 ├── LevelManager → 生成 Enemy
 ├── WeaponSystem → 生成 Bullet
 ├── PowerUpSystem → 生成 PowerUp
 ├── ParticleSystem → 生成 Particle (特效)
 ├── CollisionSystem → 碰撞检测
 ├── ScoreManager → 分数/最高分
 └── UIManager → 菜单/HUD/结束画面
```

### 关键技术点

- **固定时间步长**（60Hz）+ 累加器模式，保证不同硬件上游戏速度一致
- **对象池**（ObjectPool）管理子弹和粒子的高效复用，避免频繁 new/delete
- **双缓冲渲染**（GDI+ Bitmap + BitBlt），消除画面闪烁
- **AABB 碰撞检测**，实体间高效碰撞判断
- **视差星空**（3层不同速度），营造空间深度感

## 项目报告撰写提示

按照作业要求，报告建议包含：

1. **项目介绍** — 背景（经典射击游戏）、需求（Windows 原生 C++ 游戏）、作用（课程大作业）
2. **软件结构和设计** — 参考上方架构图，描述四层架构和模块交互
3. **AI 引导过程及代码人工调整** — 描述如何使用 AI 辅助设计架构、生成代码、修复编译错误（如 GDI+ PROPID 问题）
4. **项目演示** — 运行游戏，展示菜单、选机型、战斗、Boss 战、通关

## 许可证

本项目为课程作业，仅供学习参考。
