# 雷霆战机 — 项目进度

## 当前状态
**阶段**: 阶段 6 进行中 — 打磨与验证

## 已完成
- [x] 阶段 1: 核心引擎（Window, GameTimer, InputManager, Renderer, ResourceManager, ObjectPool）
- [x] 阶段 2: 基础实体（Entity, Player, Bullet, CollisionSystem）
- [x] 阶段 3: 敌人系统（EnemySmall, EnemyMedium, EnemyBoss, LevelManager）
- [x] 阶段 4: 游戏系统（WeaponSystem, PowerUpSystem, ParticleSystem, ScoreManager, StarField）
- [x] 阶段 5: UI（UIManager: 菜单/HUD/暂停/结束/关卡过渡/最高分）
- [x] 全部编译错误修复（GDI+ 类型歧义、PROPID 头文件、Config 命名空间等 8 个问题）
- [x] 零警告编译通过（g++ -Wall -Wextra）
- [x] Git 初始化并提交
- [x] README.md 编写
- [x] 游戏启动运行验证（3秒无崩溃，主菜单正常渲染）

## 进行中
- [ ] 完整游戏流程测试（需人工操作）
- [ ] 关卡平衡调整

## 已完成验证
- ✅ 编译零警告通过
- ✅ 游戏可启动并显示主菜单
- ✅ 窗口创建正常
- ✅ GDI+ 渲染正常
- ✅ 代码 3216 行，满足复杂度要求

## 未验证项（需人工测试）
- ⚠️ 完整游戏流程（选择机型→5关通关→死亡→返回菜单）
- ⚠️ 射击手感（子弹速度、冷却时间是否合适）
- ⚠️ 敌人 AI 行为（之字移动、瞄准射击是否正确）
- ⚠️ Boss 三阶段弹幕是否正确切换
- ⚠️ 碰撞检测是否合理（不会穿模/误判）
- ⚠️ 道具掉落概率是否平衡
- ⚠️ 最高分保存/读取是否正常
- ⚠️ 窗口关闭正常退出

## 可能需要的调整
1. 游戏难度平衡（敌人 HP、速度、子弹频率）
2. 可视区域适配（不同分辨率显示器）
3. 音效添加（如需要，Windows PlaySound API 可用）
4. 战机选择确认界面优化
