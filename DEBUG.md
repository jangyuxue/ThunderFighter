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
