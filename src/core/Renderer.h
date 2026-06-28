#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include "ResourceManager.h"

// ============================================================
// GDI+ 双缓冲渲染器
// ============================================================
class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize(HWND hWnd, int canvasWidth, int canvasHeight);
    void Shutdown();

    // 每帧调用
    void BeginFrame();   // 清空后台缓冲
    void EndFrame();     // 将后台缓冲 blit 到屏幕

    // 获取绘图接口
    Gdiplus::Graphics* GetGraphics() { return m_graphics; }
    ResourceManager&   GetResources() { return m_resources; }

    int GetCanvasWidth()  const { return m_canvasWidth; }
    int GetCanvasHeight() const { return m_canvasHeight; }

private:
    HWND                m_hWnd    = nullptr;
    int                 m_canvasWidth  = 0;
    int                 m_canvasHeight = 0;
    ULONG_PTR           m_gdiplusToken = 0;

    Gdiplus::Bitmap*    m_backBuffer = nullptr;
    Gdiplus::Graphics*  m_graphics   = nullptr;
    HDC                 m_bufferDC   = nullptr;
    HBITMAP             m_hBitmap    = nullptr;
    HBITMAP             m_hOldBitmap = nullptr;

    ResourceManager     m_resources;
};
