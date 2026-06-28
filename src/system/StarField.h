#pragma once

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>
#include <vector>
#include "config/GameConfig.h"

// ============================================================
// 视差滚动星空背景（3层）
// ============================================================
class StarField {
public:
    StarField();
    void Init();
    void Update(float dt);
    void Render(Gdiplus::Graphics& g);

private:
    struct Star {
        float x, y;
        float speed;
        float size;
        int   brightness;  // 0-255
    };

    struct StarLayer {
        std::vector<Star> stars;
        float scrollSpeed;
        float starSizeMin;
        float starSizeMax;
        int   brightnessMin;
        int   brightnessMax;
        int   count;
    };

    StarLayer m_layers[3];
};
