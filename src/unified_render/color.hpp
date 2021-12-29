#pragma once

#include <cstdint>

// Maybe make it a template so it could be either float or uint8_t
namespace UnifiedRender {
    struct Color {
        float r, g, b;
        float a = 1.f;
    public:
        Color(void);
        Color(float red, float green, float blue, float alpha = 1.f);
        ~Color(void);

        uint32_t get_value() const;
        static Color rgb8(uint8_t red, uint8_t green, uint8_t blue);
        static Color rgba8(uint8_t red, uint8_t green, uint8_t blue,uint8_t alpha);
        static Color rgba32(uint32_t argb);
        static Color lerp(Color color1, Color color2, float lamda);
    };
};
