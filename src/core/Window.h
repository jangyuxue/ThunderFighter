#pragma once

#include <windows.h>
#include <string>

// ============================================================
// Win32 窗口封装
// ============================================================
class Window {
public:
    Window();
    ~Window();

    bool Create(const std::wstring& className,
                const std::wstring& title,
                int width, int height);
    void Show(int nCmdShow);
    HWND GetHWND() const { return m_hWnd; }
    HINSTANCE GetHInstance() const { return m_hInstance; }
    int   GetWidth()  const { return m_width; }
    int   GetHeight() const { return m_height; }

private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg,
                                    WPARAM wParam, LPARAM lParam);

    HWND      m_hWnd      = nullptr;
    HINSTANCE m_hInstance = nullptr;
    int       m_width     = 0;
    int       m_height    = 0;
};
