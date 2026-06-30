# 雷霆战机（Thunder Fighter）项目报告

## 一、项目介绍

### 1.1 项目背景

《雷霆战机》是一款经典的自上而下卷轴射击游戏（Vertical Scrolling Shooter），玩家操控一架战机在垂直滚动的关卡中消灭敌人、躲避弹幕、挑战 Boss。该类游戏起源于街机时代，以紧张刺激的射击手感和丰富的弹幕设计著称，是计算机图形学、游戏物理、状态机设计等知识点的典型综合实践载体。

本项目作为 C++ 课程大作业，要求在 Windows 平台下使用原生 API 完成一款可运行的射击游戏，不依赖外部游戏引擎，重点考察学生对面向对象设计、实时循环、碰撞检测、资源管理、UI 交互等核心编程能力的掌握。

### 1.2 需求分析

项目核心需求包括：

- **基础玩法**：战机可上下左右移动，支持射击、炸弹、受击、死亡与复活；敌人按波次生成，具备不同移动与攻击模式；每关末尾出现 Boss，击败后进入下一关。
- **战机选择**：提供多种战机（速度型、均衡型、力量型），不同战机在速度、火力、生命值上存在差异。
- **关卡系统**：至少 5 个关卡，每关敌人配置、Boss 强度逐步提升。
- **商店与成长**：通过击杀敌人获得金币，可在商店购买永久能力（生命、武器、护盾、炸弹、加速等），并在下一局生效。
- **UI 界面**：包含主菜单、战机选择、关卡选择、商店、HUD、暂停、游戏结束等界面，且中文显示正常。
- **数据持久化**：金币、商店购买记录、关卡解锁进度需保存到本地，重启后仍然有效。

### 1.3 项目作用

本项目既是课程作业，也是一套可扩展的轻量级 2D 射击游戏框架：

- **教学价值**：覆盖了 C++17、Windows GDI+ 图形渲染、固定步长游戏循环、对象池、有限状态机、AABB 碰撞、文件持久化等关键技术。
- **可扩展性**：模块划分清晰，新增敌机、弹幕模式、关卡、道具均只需修改局部文件。
- **可玩性**：通过商店成长、多战机选择、5 关递进难度，提供完整的单局循环。

---

## 二、软件结构和设计

### 2.1 总体架构

项目采用经典的"引擎层 + 游戏层 + 表现层"三层结构，各层之间通过明确定义的接口交互，避免循环依赖。

```
Thunder Fighter
├── 引擎层 (core/)
│   ├── Window          —— Windows 窗口创建与消息处理
│   ├── GameTimer       —— 高精度计时器，提供固定步长循环所需的时间切片
│   ├── InputManager    —— 键盘/鼠标输入轮询与状态记录
│   ├── Renderer        —— GDI+ 初始化、双缓冲渲染、帧提交
│   ├── ObjectPool<T>   —— 固定数组对象池，供子弹/粒子复用
│   └── ResourceManager —— 字体/颜色等运行时资源管理
│
├── 游戏层
│   ├── Game            —— 主循环、状态调度、碰撞检测、实体生命周期
│   ├── entity/         —— Player、Enemy、Bullet、PowerUp、Particle
│   ├── system/         —— LevelManager、WeaponSystem、PowerUpSystem、
│   │                     ParticleSystem、ScoreManager、StarField、BulletPatterns
│   └── config/         —— GameConfig.h（全局常量）、LevelData.h（关卡数据）
│
└── 表现层 (ui/)
    ├── UIManager       —— 游戏状态机、菜单/HUD/暂停/结束渲染与输入处理
    └── Button          —— 可复用按钮控件
```

### 2.2 核心模块说明

#### 2.2.1 游戏主循环（Game）

`Game::Run()` 采用固定步长（Fixed Timestep）模式，每帧按 `FIXED_TIMESTEP`（1/60 秒）累积物理/逻辑更新，最多允许 5 次跳帧，渲染则在逻辑更新后按插值比例绘制。该模式保证游戏逻辑在不同帧率下稳定一致。

主循环根据 `UIManager` 返回的 `GameState` 分发处理：

- `HUB / LEVEL_SELECT / SHOP`：处理 UI 输入、战机选择、开始游戏。
- `PLAYING`：更新玩家、敌人、子弹、道具、粒子，执行碰撞检测。
- `PAUSED`：仅处理 UI 输入与暂停界面渲染。
- `LEVEL_TRANSITION`：处理关卡间过渡。
- `GAME_OVER`：处理结束界面输入。

#### 2.2.2 实体系统（entity/）

所有可绘制、可碰撞对象均继承自 `Entity`，基类提供位置、速度、尺寸、碰撞盒、活跃状态等通用属性，以及 `Update` / `Render` 虚函数。

- **Player**：处理移动、射击冷却、武器升级、护盾、炸弹、受伤无敌等。
- **Enemy**：基类，派生出 `EnemySmall`、`EnemyMedium`、`EnemyBoss`、`EnemyShooter`、`EnemyTank`、`EnemyElite`，分别实现差异化移动与射击判定。
- **Bullet**：玩家/敌方子弹，支持普通弹与激光两种形态。
- **PowerUp / Particle**：道具与特效粒子，由对应系统统一生成与管理。

#### 2.2.3 游戏系统（system/）

- **LevelManager**：读取 `LevelData.h` 中的关卡波次数据，控制敌人生成节奏、Boss 出场、关卡过渡。
- **WeaponSystem**：根据玩家武器等级发射不同弹幕（单发、双发、散射、激光）。
- **BulletPatterns**：独立弹幕模式库，提供 `AimedSpread`、`RadialFan`、`Spiral`、`WaveWall`、`CrossBurst` 等几何弹幕生成函数，供 `Game::UpdateEnemies` 与各敌机类型复用。
- **PowerUpSystem**：管理道具掉落与拾取效果（生命、炸弹、武器升级、护盾、加速）。
- **ParticleSystem**：管理爆炸、引擎尾焰、火花等粒子特效。
- **ScoreManager**：分数、连击、金币计算与统一存档读写。
- **StarField**：背景星空滚动效果。

#### 2.2.4 UI 系统（ui/）

`UIManager` 维护一个 `GameState` 状态机，负责所有非游戏内界面：

- 主菜单（HUB）：战机选择、开始游戏、关卡选择、商店。
- 关卡选择、商店、HUD、暂停、游戏结束。
- 按钮通过 `Button` 类统一实现，支持悬停、点击、锁定、已拥有等状态。

字体统一通过 `SystemParametersInfoW` 获取系统默认中文字体（如 `Microsoft YaHei UI`），确保中文正常显示。

#### 2.2.5 持久化（ScoreManager）

采用单文件原子写入方式，文件格式为：

```
"TSAV" | version(int) | gold(int) | totalGoldEarned(int)
       | shopOwned[8] | levelCleared[5] | levelUnlocked[5]
```

所有金币、商店购买、关卡进度通过 `SaveAll` / `LoadAll` 统一读写，避免多函数操作同一文件导致数据覆盖丢失。

### 2.3 模块交互关系

```
main.cpp
   │
   ▼
Window ───────► Game::Initialize()
                   │
                   ├── Renderer.Initialize()  ──► GDI+ 启动
                   ├── UIManager.Init()       ──► 字体/按钮/UI 状态
                   ├── UIManager.LoadData()   ──► ScoreManager.LoadAll()
                   └── StarField.Init()
                   │
                   ▼
               Game::Run()
                   │
                   ├── ProcessMessages()      ──► Window 消息泵
                   ├── FixedUpdate()          ──► Input / UIManager / Player /
                   │                              Enemies / Bullets / PowerUps /
                   │                              Particles / Collisions
                   └── Render()               ──► StarField / Entities /
                                                  ParticleSystem / UIManager
                   │
                   ▼
               Game::Shutdown()
                   │
                   └── UIManager.SaveData()   ──► ScoreManager.SaveAll()
```

### 2.4 关键技术点

- **对象池复用**：`ObjectPool<T, MaxSize>` 用于子弹和粒子，避免每帧频繁 new/delete；通过检测"槽位占用但实体已停用"来回收泄漏槽位，修复了长期游戏后子弹/特效失效的问题。
- **GDI+ 渲染**：所有绘制坐标统一使用 `float`（带 `.0f` 后缀），避免 `FillEllipse` / `FillRectangle` 等函数因 `REAL` 与 `INT` 重载产生编译歧义。
- **宽字符格式化**：在 MinGW-w64/msvcrt 环境下，`swprintf` 的 `%s` 对应 `char*`，`%ls` 才对应 `wchar_t*`；所有格式化 `wchar_t*` 的地方统一使用 `%ls`，避免中文/特殊符号乱码。
- **状态机与解耦**：`GameState` 集中管理 UI 状态，各模块只读取自己关心的状态，降低耦合。

---

## 三、AI 引导过程及代码人工调整

### 3.1 AI 引导的开发模式

本项目在开发过程中借助 AI 辅助完成以下工作：

1. **需求拆解与架构规划**：将射击游戏拆分为引擎层、游戏层、表现层，明确各模块职责。
2. **Bug 定位与根因分析**：通过系统性排查，定位了构建失败、中文乱码、存档覆盖、对象池槽位泄漏、启动状态异常等问题的真正根因。
3. **代码生成与重构**：在确认根因后，由 AI 生成最小化修复代码，并补充单元测试或冒烟验证。
4. **扩展设计**：新增敌机类型与弹幕模式时，先由 AI 给出可扩展方案，再按方案逐步实施。

### 3.2 主要问题与修复路径

| 问题 | 根因 | 修复方式 |
|------|------|----------|
| 中文乱码 | Makefile 缺失 `-Isrc` 与 `src/ui/Button.cpp`，导致修复从未编译进 exe | 修正 Makefile，补全源文件与头文件搜索路径 |
| 商店购买不持久 | `SaveProgress` 用 `fopen("wb")` 截断文件，覆盖了 `SaveShopData` 追加的商店数据 |  redesign 为单一原子存档格式 `SaveAll` / `LoadAll` |
| 射击渐进失效 | 对象池 `Acquire` 只检查槽位标记，未处理实体已停用但槽位未释放的泄漏 | `Acquire` 改为检查 `!m_active[i] \|\| !m_objects[i].IsActive()` |
| 战机名乱码 | `swprintf` 中 `%s` 在 msvcrt 下被当作 `char*`，错误读取 `wchar_t*` | 所有格式化 `wchar_t*` 处改为 `%ls` |
| 启动误切关卡选择 | `Game::Run` 未预热输入，第一帧按住鼠标被误判为点击 | `Game::Run` 开头加 `m_input.Update()`，并在 `UIManager::Init` 显式重置为 HUB |
| 按钮不居中 | `Button` 以中心为坐标，但初始化代码按左上角计算 | `hx = cx - btnW * 0.5f` 改为 `hx = cx` |

### 3.3 代码人工调整

AI 生成的代码在合并前均经过人工审查与调整，主要调整点包括：

- **参数微调**：敌机 HP、速度、射击间隔、弹幕速度等数值根据实际体验进行人工校准，避免过难或过简单。
- **视觉细节**：敌机渲染颜色、尺寸、血条位置等根据屏幕整体效果进行微调。
- **关卡波次**：`LEVELS` 中各波次的敌人类型、数量、间隔由开发者根据测试感受进行平衡。
- **布局优化**：HUB 按钮排列、间距、字号等经过多轮人工验证后确定最终方案。
- **最终清理**：删除临时截图、构建产物、旧格式存档等不需要进入版本控制的文件，仅保留源代码与必要文档。

### 3.4 交付物说明

最终交付物包含：

- `src/` 下的完整 C++ 源代码
- `Makefile` 构建脚本
- `README.md` 项目说明
- `DEBUG.md` 调试记录
- `REPORT.md` 本报告
- `.gitignore` 忽略规则

所有临时文件、构建产物、游戏运行时存档均已排除在版本控制之外。
