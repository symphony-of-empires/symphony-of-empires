#pragma once

#include <cstdint>

// Maybe make it a template so it could be either float or uint8_t
namespace UI {
    struct Color {
    public:
        Color(float red, float green, float blue): r{ red }, g{ green }, b{ blue } {}
        Color(float red, float green, float blue, float alpha): r{ red }, g{ green }, b{ blue }, a{ alpha } {}
        float r, g, b;
        float a = 1.f;
        uint32_t get_value() const;
        static Color rgb8(uint8_t red, uint8_t green, uint8_t blue);
        static Color rgba8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
        static Color rgba32(uint32_t argb);
        static Color lerp(Color color1, Color color2, float lamda);
    };
}