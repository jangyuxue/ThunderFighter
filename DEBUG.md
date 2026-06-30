# 雷霆战机 — 调试记录

## #1: GDI+ 头文件 PROPID 类型缺失
- **触发**: 首次编译，Player.cpp 中 `Gdiplus::Color` 不可用
- **根因**: MinGW-w64 的 GDI+ 头文件需要 `objbase.h` 提供 `PROPID` 类型，必须先于 `gdiplus.h` 包含
- **修复**: 在所有包含 `gdiplus.h` 的头文件中添加前置 `#include <objbase.h>`
- **影响文件**: Entity.h, Game.h, Renderer.h, ResourceManager.h, StarField.h, UIManager.h
- **验证**: 编译通过
- **教训**: MinGW 环境下 include 顺序必须是 `windows.h` → `objbase.h` → `gdiplus.h`

## #2: GDI+ 函数调用类型歧义
- **触发**: FillEllipse/FillRectangle 调用中混用 float 和 int 参数
- **根因**: GDI+ 存在 `(REAL,REAL,REAL,REAL)` 和 `(INT,INT,INT,INT)` 两个重载，混用导致 ADL 歧义
- **修复**: 所有坐标参数统一使用 float（整数加 `.0f` 后缀）
- **影响文件**: Player.cpp, Bullet.cpp, Enemy.cpp, UIManager.cpp
- **验证**: 编译通过

## #3: Config 命名空间不可见
- **触发**: Bullet.cpp 中 `Config::LASER_WIDTH` 等未声明
- **根因**: 实体 cpp 文件只 include 了各自的 .h，而 .h 中没有 include GameConfig.h
- **修复**: 在 Entity.h 基类中添加 `#include "config/GameConfig.h"`
- **验证**: 编译通过

## #4: Player.h GetWeaponLevel() 重复声明
- **触发**: 编译错误，同一函数声明两次
- **根因**: Player.h 中两处声明了 `GetWeaponLevel()`
- **修复**: 删除重复的声明
- **验证**: 编译通过

## #5: LevelData.h 缺少 vector 头文件
- **触发**: `std::vector` 未定义
- **修复**: 添加 `#include <vector>`
- **验证**: 编译通过

## #6: Enemy.cpp swprintf 未声明
- **触发**: Boss 渲染中使用 swprintf
- **修复**: 添加 `#include <cstdio>`
- **验证**: 编译通过

## #7: Game.cpp 缺少 algorithm 头文件
- **触发**: `std::remove_if` 未找到
- **修复**: 添加 `#include <algorithm>`
- **验证**: 编译通过

## #8: ResourceManager.cpp 反斜杠路径
- **触发**: `#include "core\ResourceManager.h"` 的反斜杠导致 include 失败
- **修复**: 改为正斜杠 `"core/ResourceManager.h"`
- **验证**: 编译通过

## #9: 中文乱码 — 真正根因（前3次修复均未生效）
- **触发**: 用户报告界面中文持续乱码，已修3次（MakeFont 多字体尝试、SystemParametersInfoW 系统字体）仍未解决
- **根因**: Makefile 缺失 `-Isrc`（头文件按 `core/X.h` 引用无法解析）且 SOURCES 漏列 `src/ui/Button.cpp`，导致 `mingw32-make` 从未成功构建。用户一直在运行**旧的乱码 exe**，所有"字体修复"都没编译进可执行文件，故乱码"修了3次仍存在"。
  - 排查过程（已用测试逐项排除，非猜测）:
    1. 源码编码: 全部源文件为 UTF-8 无 BOM（Python 严格解码验证）
    2. 字符串字面量: g++ 14.2.0 默认即按 UTF-8 解码，wchar_t 值正确（`雷霆战机`=96F7 9706 6218 673A），与 locale/`-finput-charset` 标志无关 → 编码非根因
    3. GDI+ 初始化: Renderer::Initialize 先于 UIManager::Init 调用 GdiplusStartup，顺序正确
    4. 字体加载: SystemParametersInfoW 返回 `Microsoft YaHei UI`，Font 创建 status=Ok、family 正确、IsAvailable=1
    5. 渲染管线: 双缓冲 GDI+ Graphics + AntiAlias 文本提示，正确
    6. 构建: Makefile 编不过（缺 -Isrc + 漏 Button.cpp）→ 真正阻断点
- **修复**: Makefile CXXFLAGS 增加 `-Isrc -finput-charset=UTF-8 -fexec-charset=UTF-8`；SOURCES 补 `src/ui/Button.cpp`
- **验证**: `mingw32-make` 构建成功生成 ThunderFighter.exe；无头 DrawString 测试确认中文渲染为像素（雷霆战机=2634px、商店=1249px、AAAA=1348px，与字符数成正比）
- **教训**: "修了仍不生效"时，第一反应应是确认修复是否真正编译/部署生效，而非继续改源码。多次无效修复通常意味着构建链路本身断裂。

## #10: 商店购买能力不持久化 + 关卡解锁丢失 + 金币每局被重发
- **触发**: 花金币买能力后退出重进，金币扣了但能力没保存；通关后重启关卡又锁上
- **根因(商店)**: `SaveShopData` 以 `fopen("rb+")` 追加 "SHOP" 块到 thunder.sav，但 `SaveProgress`（在 `SaveData` 中紧随其后、且独立调用）用 `fopen("wb")` **截断整个文件**，把 SHOP 块冲掉。git status 证实 thunder.sav 仅 12 字节(=TSAV+gold+totalGold)，SHOP 块从未落盘。`LoadShopData` 扫不到 "SHOP" 标记 → m_shopOwned 全 false → 能力丢失。金币因 SaveProgress 最后写入而存活。
- **根因(关卡)**: `m_levelCleared`/`m_levelUnlocked` 从不写入/读取磁盘，`OnLevelCleared` 仅改内存，重启即丢。
- **根因(金币重发)**: 构造函数每次加载都执行 `if(m_gold<1000) m_gold=1000`，注释写"开局至少1000"意图是首次赠送，但代码每局重发 → 花掉后重进金币被退回。
- **修复**: 重设计为单一原子存档格式 `TSAV|version|gold|totalGold|shopOwned[8]|levelCleared[5]|levelUnlocked[5]`(34字节)，单一写入函数 `SaveAll`/`LoadAll`，删除旧的 SaveProgress/SaveShopData。购买后立即 `SaveData`。1000金币仅在无存档(首次启动)时赠送。旧档(12字节无version)自动迁移保留金币。
- **验证**: 无头往返测试 6/6 通过（金币/商店能力/通关记录/解锁记录持久化、未购买保持false、文件34字节）。
- **教训**: 多个写函数操作同一文件且语义不一致(追加 vs 截断)是数据丢失的温床。同一持久化目标应只有单一写入路径。

## #11: 商店8个道具仅3个生效 + 内联字体可能缺字形
- **触发**: 用户购买能力后即使存档保留，部分能力（额外炸弹/永久加速/最大生命/起始炸弹/起始护盾）开局仍不生效
- **根因(生效)**: Game.cpp 仅按 `m_playerUpgrades` 位标记应用 3 项（id0/1/2 对应 bit1/2/4），id3/4/5/6/7 设置了 `m_shopOwned` 但从未映射到位标记、也从未应用。id4(永久加速)的 bit8 设了却无人读取。`m_playerUpgrades` 这个位标记本身只编码 4/8 项，是"5个道具无效"的根因。
- **根因(字体)**: 内联字体硬编码 `L"Microsoft YaHei"`，本机实测该 family 名返回空(非系统消息字体 `Microsoft YaHei UI`)，存在回退到缺 CJK 字形字体的风险。
- **修复**:
  1. 删除 `m_playerUpgrades` 位标记机制（含成员、GetPlayerUpgrades、LoadData 重建、BuyShopItem 置位）。
  2. Game.cpp 改为按 `HasShopItem(i)` 逐项应用全部8项，新增 `Player::ApplyPermanentSpeed`(提升baseSpeed) 与 `Player::IncreaseMaxLives`(提升上限+补命)；AddExtraLife 改用运行时 `m_maxLives` 上限(原用编译期常量)。
  3. UIManager 缓存系统字体名 `m_fontName`，所有内联 Font 改用它，确保 CJK 字形。
  4. 商店预览改读 `m_shopOwned`，准确展示生命/上限/武器/护盾/炸弹/加速。
- **验证**: 无头测试 6/6 通过（8项道具全部作用于Player、最大生命上限真提升至6）；`mingw32-make` 构建成功；exe 启动冒烟测试进程存活无崩溃。
- **教训**: 用位标记压缩状态时，若新增项忘记扩展位标记映射，会出现"已购但不生效"的静默失败。直接以原始布尔数组为单一数据源更可靠。

## #12: 射击/特效渐进式失效 — ObjectPool 槽位泄漏
- **触发**: 用户报告"打着打着进行到一定地步就不发射子弹了"
- **根因**: ObjectPool 有两套独立的 active 状态——池的 `m_active[MaxSize]` 槽位标记 vs 实体自身的 `Entity::m_active`。游戏中子弹/粒子出屏或寿命结束只调 `Entity::Deactivate()` (置实体 m_active=false)，**从不调 `pool.Release()`** (全代码 Release 仅在 ReleaseAll 开局/关程序时调用)。导致池槽位标记永不归零、槽位逐渐全部占用、`Acquire` 最终返回 nullptr、`WeaponSystem::Fire` 静默失败、子弹不再发射。子弹池(300)与粒子池(500)同源泄漏(爆炸/引擎拖尾也会渐进式消失)。
- **为何渐进式**: 需累积到池满。约每0.15s发1-5发子弹、出屏后永久占槽，数百发后(数十秒)池满。符合"进行到一定地步才失效"。
- **修复**: ObjectPool::Acquire 可复用条件由 `!m_active[i]` 改为 `!m_active[i] || !m_objects[i].IsActive()`。复用"占用但已停用"的槽位时先逻辑释放(保持 m_activeCount 自洽)。无需改任何调用方。
- **验证**: 失败测试复现 bug(退出码1)→修复后 6/6 通过(池满可复用、持续射击10000次无泄漏、活跃对象不被复用、全活跃池满返回nullptr、ReleaseAll归零); clean重编构建成功; exe启动冒烟测试存活无崩溃。
- **教训**: 对象池若用独立于实体生命周期的"槽位标记"管理复用，必须保证实体停用时同步释放槽位(或让 Acquire 直接依据实体活跃状态判定)。两套状态脱钩必然导致槽位泄漏。

## #13: 战机名行 emoji 乱码（仅 emoji 显示异常）
- **触发**: 用户报告战机选择处乱码；经确认中文正常，仅 emoji(⚡🌪🔥) 显示异常
- **根因**: 辅助平面 emoji(U+1F000+，需 surrogate pair) 依赖 emoji 字体(Segoe UI Emoji)渲染。部分 Windows 系统/精简字体环境缺失该字体 → 显示为乱码/豆腐块。wchar_t 字面量本身正确(D83C DF2A 等 surrogate pair 完整)，g++ 解码正确，GDI+ 加载正常，问题在字形缺失。本机有 fallback 故测试能渲染，用户机缺失。
- **修复**: 移除所有辅助平面 emoji（战机名⚡🌪🔥、商店/任务💰、商店预览✓✗、任务✓），改用纯中文。保留 BMP 几何符号(◀▶★)——所有字体均支持。成就界面🏅🔒随成就系统删除一并移除。
- **验证**: 无头渲染测试确认去emoji后纯中文战机名3/3正常渲染(像素数3039/3121/3388)；构建成功；启动冒烟存活无崩溃。
- **教训**: emoji 跨系统显示不一致，游戏 UI 应避免依赖 emoji 字体；用纯文字或 BMP 符号更可靠。

## #14: 删除排行榜与成就系统
- **触发**: 用户要求删除排行榜、成就功能
- **变更**:
  - 排行榜: 移除 HIGH_SCORE 状态、RenderHighScore、ScoreManager 的 HighScoreEntry/LoadHighScores/SaveHighScores/IsHighScore/InsertHighScore/GetHighScores/m_highScores、Game.cpp 的 SaveHighScores 调用
  - 成就: 移除 ACHIEVEMENTS 状态、RenderAchievements、UpdateAchievementsButtons、m_achievementButtons、m_achievementUnlocked、成就通知计时器
  - HUB 布局: 6按钮(2列3行) → 4按钮(2列2行): 开始游戏/关卡选择/商店/任务
- **注意**: 排行榜从未实际可用(InsertHighScore 从无调用)，成就从未解锁(无触发代码)——删除的是未完成的功能骨架，无功能损失
- **验证**: 构建成功(exe 258KB→254KB)；零残留引用(已 grep 确认)；启动冒烟存活无崩溃

## #15: 战机名乱码新根因 — swprintf %s 格式化 wchar_t* 在 MinGW-w64/msvcrt 下被解释为 char*
- **触发**: 用户截图显示战机选择处绿色大字 "飓风 均衡型" 显示为 "ÓIÎ GWaW"，但下方描述 "全能 · 标准火力 · 3生命 · 绿色涂装" 正常；标题、按钮等中文均正常
- **根因**: MinGW-w64 链接的 msvcrt 中，`swprintf` 的 `%s` 格式符对应 **`char*`**，而不是 `wchar_t*`。代码中用 `swprintf(buf, 64, L"%s  %s", names[i], types[i])` 格式化 `wchar_t*` 战机名/类型时，msvcrt 把 `wchar_t*` 指针当作 `char*` 读取，读取到 UTF-16LE 字节流（如 "均衡型" = 47 57 61 88 8B 57），再零扩展为 wchar_t 输出，结果渲染成西欧字符（G/W/a/ˆ/‹），与用户截图中的 "GWaW" 完全吻合。同一文件中直接传给 `Gdiplus::DrawString` 的字面量中文正常，因为不存在 `swprintf` 的中间错误转换。
- **为何之前误以为是 emoji**: 同一位置原有 emoji(⚡🌪🔥)，先去掉了 emoji，但乱码仍在；说明真正根因不是 emoji 字体缺失，而是 `%s` 格式化错误。emoji 只是让问题更复杂。
- **影响位置**: UIManager.cpp 中所有用 `%s` 格式化 `wchar_t*` 的地方：
  - 327行 战机名+类型
  - 387行 关卡列表（关卡名+星级）
  - 430行 商店预览（战机名）
  - 501行 HUD 关卡名
- **修复**: 将上述 `%s` 全部改为 `%ls`（C 标准中长字符串格式，明确对应 `wchar_t*`）。
- **验证**:
  1. 最小化测试程序复现：`swprintf(..., L"%s", L"飓风")` 输出乱码；`%ls` 输出正确 Unicode 码点 98D3 98CE。
  2. 格式化后 buf 的 wchar_t 值检查："飓风 均衡型" = 98D3 98CE 0020 0020 5747 8861 578B，与正确码点一致。
  3. 新构建的 exe 中 UTF-16LE 字符串搜索确认包含 "闪电/飓风/烈焰/速度型/均衡型/力量型/地球轨道/小行星带"。
  4. `mingw32-make clean && mingw32-make` 构建成功；启动冒烟测试进程存活无崩溃。
- **教训**: Windows/msvcrt 宽字符格式化是陷阱：`swprintf`/`wprintf` 中 `%s` 通常指 `char*`，`%ls` 才是 `wchar_t*`。跨平台代码若曾在 glibc 下 `%s` 工作，移植到 MinGW-w64 后必须改为 `%ls`。所有宽字符串格式化都应使用 `%ls` 并逐一审核。
