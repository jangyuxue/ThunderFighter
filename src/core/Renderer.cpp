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

    // 创建后台缓冲位图
    m_backBuffer = new Gdiplus::Bitmap(canvasWidth, canvasHeight,
                                        PixelFormat32bppARGB);
    if (m_backBuffer->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    // 创建 Graphics 对象（绘制到后台缓冲）
    m_graphics = new Gdiplus::Graphics(m_backBuffer);
    if (m_graphics->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    // 设置渲染质量
    m_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    m_graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
    m_graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

    // 创建兼容 DC（用于 BitBlt）
    HDC screenDC = GetDC(m_hWnd);
    m_bufferDC = CreateCompatibleDC(screenDC);

    // 从 GDI+ 位图获取 HBITMAP
    m_backBuffer->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &m_hBitmap);
    m_hOldBitmap = (HBITMAP)SelectObject(m_bufferDC, m_hBitmap);
    ReleaseDC(m_hWnd, screenDC);

    return true;
}

void Renderer::BeginFrame() {
    // 深空蓝黑色背景
    m_graphics->Clear(Gdiplus::Color(5, 5, 18));
}

void Renderer::EndFrame() {
    // 将后台缓冲 BitBlt 到屏幕
    HDC screenDC = GetDC(m_hWnd);
    BitBlt(screenDC, 0, 0, m_canvasWidth, m_canvasHeight,
           m_bufferDC, 0, 0, SRCCOPY);
    ReleaseDC(m_hWnd, screenDC);
}

void Renderer::Shutdown() {
    // 清理 GDI 资源
    if (m_bufferDC) {
        if (m_hBitmap) {
            SelectObject(m_bufferDC, m_hOldBitmap);
            DeleteObject(m_hBitmap);
            m_hBitmap = nullptr;
            m_hOldBitmap = nullptr;
        }
        DeleteDC(m_bufferDC);
        m_bufferDC = nullptr;
    }

    // 清理 GDI+ 资源
    m_resources.Clear();
    delete m_graphics;
    m_graphics = nullptr;
    delete m_backBuffer;
    m_backBuffer = nullptr;

    // 关闭 GDI+
    if (m_gdiplusToken != 0) {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
}
