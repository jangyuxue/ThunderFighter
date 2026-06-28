#pragma once

namespace Gdiplus {
    class Graphics;
}

// ============================================================
// 可渲染对象接口
// ============================================================
class Renderable {
public:
    virtual ~Renderable() = default;
    virtual void Render(Gdiplus::Graphics& g) = 0;
};
