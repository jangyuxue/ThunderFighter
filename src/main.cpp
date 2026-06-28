// ============================================================
// 雷霆战机 (Thunder Fighter) — 程序入口
// ============================================================

#include <windows.h>
#include "Game.h"

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/,
                   LPSTR /*lpCmdLine*/, int nCmdShow) {
    // 创建窗口
    Window window;
    if (!window.Create(L"ThunderFighterClass", L"Thunder Fighter - 雷霆战机",
                       Config::CANVAS_WIDTH, Config::CANVAS_HEIGHT)) {
        MessageBoxW(nullptr, L"Failed to create window!", L"Error",
                    MB_OK | MB_ICONERROR);
        return 1;
    }
    window.Show(nCmdShow);

    // 初始化游戏
    Game game;
    if (!game.Initialize(window.GetHWND())) {
        MessageBoxW(nullptr, L"Failed to initialize game!", L"Error",
                    MB_OK | MB_ICONERROR);
        return 1;
    }

    // 运行主循环
    game.Run();

    // 清理
    game.Shutdown();
    return 0;
}
