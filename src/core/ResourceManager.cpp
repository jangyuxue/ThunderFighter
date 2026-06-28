#include "core/ResourceManager.h"

ResourceManager::~ResourceManager() {
    Clear();
}

Gdiplus::SolidBrush* ResourceManager::GetBrush(Gdiplus::Color color) {
    BrushKey key{ color.GetValue() };
    auto it = m_brushes.find(key);
    if (it != m_brushes.end()) {
        return it->second;
    }
    auto* brush = new Gdiplus::SolidBrush(color);
    m_brushes[key] = brush;
    return brush;
}

Gdiplus::Pen* ResourceManager::GetPen(Gdiplus::Color color, float width) {
    PenKey key{ color.GetValue(), width };
    auto it = m_pens.find(key);
    if (it != m_pens.end()) {
        return it->second;
    }
    auto* pen = new Gdiplus::Pen(color, width);
    m_pens[key] = pen;
    return pen;
}

Gdiplus::Font* ResourceManager::GetFont(const std::wstring& name, float size,
                                         int style) {
    FontKey key{ name, size, style };
    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second;
    }
    auto* font = new Gdiplus::Font(name.c_str(), size, style);
    m_fonts[key] = font;
    return font;
}

void ResourceManager::Clear() {
    for (auto& pair : m_brushes) delete pair.second;
    for (auto& pair : m_pens)    delete pair.second;
    for (auto& pair : m_fonts)   delete pair.second;
    m_brushes.clear();
    m_pens.clear();
    m_fonts.clear();
}
