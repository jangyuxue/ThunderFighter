#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include <unordered_map>
#include <string>

// ============================================================
// GDI+ 资源缓存 — 避免每帧创建 Pen/Brush/Font
// ============================================================
class ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager();

    Gdiplus::SolidBrush* GetBrush(Gdiplus::Color color);
    Gdiplus::Pen*        GetPen(Gdiplus::Color color, float width = 1.0f);
    Gdiplus::Font*       GetFont(const std::wstring& name, float size,
                                 int style = Gdiplus::FontStyleRegular);

    void Clear();

private:
    // 使用 COLORREF 作为 Brush 键（颜色 + alpha）
    struct BrushKey {
        Gdiplus::ARGB argb;
        bool operator==(const BrushKey& o) const { return argb == o.argb; }
    };
    struct BrushKeyHash {
        size_t operator()(const BrushKey& k) const { return k.argb; }
    };

    // Pen 键
    struct PenKey {
        Gdiplus::ARGB argb;
        float width;
        bool operator==(const PenKey& o) const {
            return argb == o.argb && width == o.width;
        }
    };
    struct PenKeyHash {
        size_t operator()(const PenKey& k) const {
            return k.argb ^ (static_cast<size_t>(k.width * 100) << 16);
        }
    };

    // Font 键
    struct FontKey {
        std::wstring name;
        float size;
        int style;
        bool operator==(const FontKey& o) const {
            return name == o.name && size == o.size && style == o.style;
        }
    };
    struct FontKeyHash {
        size_t operator()(const FontKey& k) const {
            size_t h = std::hash<std::wstring>()(k.name);
            h ^= static_cast<size_t>(k.size * 100) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= static_cast<size_t>(k.style) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    std::unordered_map<BrushKey, Gdiplus::SolidBrush*, BrushKeyHash> m_brushes;
    std::unordered_map<PenKey, Gdiplus::Pen*, PenKeyHash> m_pens;
    std::unordered_map<FontKey, Gdiplus::Font*, FontKeyHash> m_fonts;
};
