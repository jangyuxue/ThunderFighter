#include "core/Renderer.h"
#include <cstdio>

Renderer::Renderer() = default;

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(HWND hWnd, int canvasWidth, int canvasHeight) {
    m_hWnd = hWnd;
    m_canvasWidth  = canvasWidth;
    m_canvasHeight = canvasHeight;

    // 初始化 GDI+
    Gdiplus::GdiplusStartupInput gdiInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiInput, nullptr);
    if (m_gdiplusToken == 0) {
        return false;
    }

    // ---- 正确的双缓冲方式：使用 GDI 位图 + GDI+ Graphics ----
    HDC screenDC = GetDC(m_hWnd);

    // 1. 创建与屏幕兼容的内存 DC
    m_bufferDC = CreateCompatibleDC(screenDC);
    if (!m_bufferDC) {
        ReleaseDC(m_hWnd, screenDC);
        return false;
    }

    // 2. 创建与屏幕兼容的位图
    m_hBitmap = CreateCompatibleBitmap(screenDC, canvasWidth, canvasHeight);
    if (!m_hBitmap) {
        DeleteDC(m_bufferDC);
        m_bufferDC = nullptr;
        ReleaseDC(m_hWnd, screenDC);
        return false;
    }

    // 3. 将位图选入内存 DC
    m_hOldBitmap = (HBITMAP)SelectObject(m_bufferDC, m_hBitmap);

    ReleaseDC(m_hWnd, screenDC);

    // 4. 创建 GDI+ Graphics，目标为内存 DC
    //    所有 GDI+ 绘制直接写入 GDI 位图，无需 GetHBITMAP 快照
    m_graphics = new Gdiplus::Graphics(m_bufferDC);
    if (m_graphics->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    // 设置渲染质量
    m_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    m_graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    m_graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

    return true;
}

void Renderer::BeginFrame() {
    if (!m_graphics) return;
    m_graphics->Clear(Gdiplus::Color(5, 5, 18));
}

void Renderer::EndFrame() {
    // 确保所有 GDI+ 绘制命令已提交到 GDI 位图
    m_graphics->Flush(Gdiplus::FlushIntentionSync);

    // 将内存 DC 的内容 BitBlt 到屏幕
    HDC screenDC = GetDC(m_hWnd);
    BitBlt(screenDC, 0, 0, m_canvasWidth, m_canvasHeight,
           m_bufferDC, 0, 0, SRCCOPY);
    ReleaseDC(m_hWnd, screenDC);
}

void Renderer::Shutdown() {
    // 清理 GDI+ Graphics
    delete m_graphics;
    m_graphics = nullptr;

    // 清理 GDI 资源
    if (m_bufferDC) {
        if (m_hOldBitmap) {
            SelectObject(m_bufferDC, m_hOldBitmap);
            m_hOldBitmap = nullptr;
        }
        if (m_hBitmap) {
            DeleteObject(m_hBitmap);
            m_hBitmap = nullptr;
        }
        DeleteDC(m_bufferDC);
        m_bufferDC = nullptr;
    }

    m_resources.Clear();

    // 关闭 GDI+
    if (m_gdiplusToken != 0) {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}
