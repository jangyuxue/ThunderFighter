# Thunder Fighter (雷霆战机) — Agent 工作指南

## 项目概述
C++ 课程大作业：自上而下卷轴射击游戏（类似雷电/雷霆战机）。

- **语言**: C++17
- **编译器**: g++ 14.2.0 (MinGW-w64)
- **图形**: Windows GDI+（零外部依赖）
- **构建**: `g++ -std=c++17 -O2 -I src src/**/*.cpp -o ThunderFighter.exe -lgdiplus -lgdi32 -luser32 -lkernel32 -mwindows`

## 目录结构
```
cpp_finalwork/
├── ThunderFighter.exe          # 编译产物
├── Makefile                    # 构建脚本
├── src/
│   ├── main.cpp                # WinMain 入口
│   ├── Game.h/cpp              # 游戏核心（主循环、碰撞、渲染协调）
│   ├── core/                   # 引擎层：Window, GameTimer, InputManager, Renderer, ObjectPool, ResourceManager
│   ├── entity/                 # 游戏对象：Entity, Player, Bullet, Enemy, PowerUp, Particle
│   ├── system/                 # 游戏系统：StarField, LevelManager, WeaponSystem, PowerUpSystem, ParticleSystem, ScoreManager
│   ├── ui/                     # 界面：UIManager (菜单/HUD/暂停/结束)
│   └── config/                 # 配置：GameConfig.h (常量), LevelData.h (关卡数据)
├── AGENT.md                    # 本文件
├── PROCESSING.md               # 进度记录
├── DEBUG.md                    # 调试记录
├── .gitignore
└── README.md
```

## 工作规则
1. **修改前先编译** — 确认当前状态可构建
2. **最小变更** — 修改单个文件后立即重新编译验证
3. **提交纪律** — 每完成一个阶段 git commit
4. **GDI+ 包含顺序** — `windows.h` → `objbase.h` → `gdiplus.h`（必须保持此顺序）
5. **类型一致性** — GDI+ 绘图函数参数统一使用 float（加 `.0f`），避免歧义
6. **编码风格** — 遵循项目既有的命名规范（PascalCase 类名，camelCase 方法，m_ 成员前缀）

## 常用命令
```bash
# 完整构建
g++ -std=c++17 -Wall -Wextra -O2 -I src src/*.cpp src/core/*.cpp src/entity/*.cpp src/system/*.cpp src/ui/*.cpp -o ThunderFighter.exe -lgdiplus -lgdi32 -luser32 -lkernel32 -mwindows

# 运行
./ThunderFighter.exe
```

## 修改边界
- ✅ 可修改：任何 .h/.cpp 文件、关卡数据、游戏常量
- ⚠️ 谨慎修改：ObjectPool.h（模板，影响所有实体）、Entity.h（基类）
- ❌ 不可修改：编译器的 include/lib 路径
