#include "core/Window.h"

Window::Window() = default;

Window::~Window() {
    if (m_hWnd) {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
}

bool Window::Create(const std::wstring& className,
                    const std::wstring& title,
                    int width, int height) {
    m_hInstance = GetModuleHandle(nullptr);
    m_width  = width;
    m_height = height;

    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = m_hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = className.c_str();

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    // 计算实际窗口大小（使客户区等于指定宽高）
    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    int winW = rect.right - rect.left;
    int winH = rect.bottom - rect.top;

    // 居中窗口
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenW - winW) / 2;
    int y = (screenH - winH) / 2;

    m_hWnd = CreateWindowExW(
        0,
        className.c_str(),
        title.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, winW, winH,
        nullptr, nullptr, m_hInstance, nullptr
    );

    return m_hWnd != nullptr;
}

void Window::Show(int nCmdShow) {
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg,
                                  WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        // 阻止 Alt+F4 之外的系统按键导致程序暂停
        if (wParam == VK_F4 && (GetKeyState(VK_MENU) & 0x8000)) {
            PostQuitMessage(0);
            return 0;
        }
        break;
    case WM_ERASEBKGND:
        // 禁止系统擦除背景，避免闪烁
        return 1;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
