#include "system/StarField.h"
#include <random>
#include <cmath>

StarField::StarField() {
    Init();
}

void StarField::Init() {
    std::mt19937 rng(42);  // 固定种子，每次启动一致

    // 3层：远（慢）→ 中 → 近（快）
    m_layers[0].count = 50;
    m_layers[0].scrollSpeed = 20.0f;
    m_layers[0].starSizeMin = 0.8f;
    m_layers[0].starSizeMax = 1.2f;
    m_layers[0].brightnessMin = 60;
    m_layers[0].brightnessMax = 120;

    m_layers[1].count = 35;
    m_layers[1].scrollSpeed = 45.0f;
    m_layers[1].starSizeMin = 1.2f;
    m_layers[1].starSizeMax = 1.8f;
    m_layers[1].brightnessMin = 100;
    m_layers[1].brightnessMax = 180;

    m_layers[2].count = 20;
    m_layers[2].scrollSpeed = 80.0f;
    m_layers[2].starSizeMin = 1.6f;
    m_layers[2].starSizeMax = 2.5f;
    m_layers[2].brightnessMin = 140;
    m_layers[2].brightnessMax = 240;

    for (int l = 0; l < 3; ++l) {
        auto& layer = m_layers[l];
        layer.stars.resize(layer.count);
        for (int i = 0; i < layer.count; ++i) {
            Star& s = layer.stars[i];
            s.x = static_cast<float>(rng() % Config::CANVAS_WIDTH);
            s.y = static_cast<float>(rng() % Config::CANVAS_HEIGHT);
            s.speed = layer.scrollSpeed + (rng() % 20 - 10);
            s.size = layer.starSizeMin
                     + static_cast<float>(rng() % 100) / 100.0f
                     * (layer.starSizeMax - layer.starSizeMin);
            s.brightness = layer.brightnessMin
                           + rng() % (layer.brightnessMax - layer.brightnessMin);
        }
    }
}

void StarField::Update(float dt) {
    for (int l = 0; l < 3; ++l) {
        for (auto& s : m_layers[l].stars) {
            s.y += s.speed * dt;
            if (s.y > Config::CANVAS_HEIGHT) {
                s.y = -5.0f;
                s.x = static_cast<float>(rand() % Config::CANVAS_WIDTH);
            }
        }
    }
}

void StarField::Render(Gdiplus::Graphics& g) {
    for (int l = 0; l < 3; ++l) {
        for (const auto& s : m_layers[l].stars) {
            BYTE b = static_cast<BYTE>(s.brightness);
            Gdiplus::Color color(b, b, b);
            Gdiplus::SolidBrush brush(color);
            g.FillEllipse(&brush, s.x - s.size * 0.5f, s.y - s.size * 0.5f,
                          s.size, s.size);
        }
    }
}
