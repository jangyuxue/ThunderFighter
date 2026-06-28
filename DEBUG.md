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
