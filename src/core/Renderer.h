#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include "ResourceManager.h"

// ============================================================
// GDI+ 双缓冲渲染器
// 使用 GDI 位图 + GDI+ Graphics（非 GDI+ Bitmap），
// 绘制直接写入 GDI 位图，BitBlt 无需 GetHBITMAP 快照
// ============================================================
class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize(HWND hWnd, int canvasWidth, int canvasHeight);
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    Gdiplus::Graphics* GetGraphics() { return m_graphics; }
    ResourceManager&   GetResources() { return m_resources; }

    int GetCanvasWidth()  const { return m_canvasWidth; }
    int GetCanvasHeight() const { return m_canvasHeight; }

private:
    HWND             m_hWnd          = nullptr;
    int              m_canvasWidth   = 0;
    int              m_canvasHeight  = 0;
    ULONG_PTR        m_gdiplusToken  = 0;

    // GDI 双缓冲（正确方式：GDI 位图 + GDI+ Graphics）
    HDC              m_bufferDC      = nullptr;
    HBITMAP          m_hBitmap       = nullptr;
    HBITMAP          m_hOldBitmap    = nullptr;
    Gdiplus::Graphics* m_graphics    = nullptr;

    ResourceManager  m_resources;
};
