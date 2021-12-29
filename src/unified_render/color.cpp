#include "unified_render/color.hpp"

UnifiedRender::Color::Color(void)
    : r{ 0.f },
    g{ 0.f },
    b{ 0.f }
{

}

UnifiedRender::Color::Color(float red, float green, float blue, float alpha)
    : r{ red },
    g{ green },
    b{ blue },
    a{ alpha }
{

}

UnifiedRender::Color::~Color(void) {

}

UnifiedRender::Color UnifiedRender::Color::rgba8(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    return UnifiedRender::Color(red / 256.f, green / 256.f, blue / 256.f, alpha / 256.f);
}

UnifiedRender::Color UnifiedRender::Color::rgb8(uint8_t red, uint8_t green, uint8_t blue) {
    return UnifiedRender::Color(red / 256.f, green / 256.f, blue / 256.f);
}

UnifiedRender::Color UnifiedRender::Color::rgba32(uint32_t argb) {
    float a = ((argb >> 24) & 0xff) / 256.f;
    float b = ((argb >> 16) & 0xff) / 256.f;
    float g = ((argb >> 8) & 0xff) / 256.f;
    float r = (argb & 0xff) / 256.f;
    return UnifiedRender::Color(r, g, b, a);
}

uint32_t UnifiedRender::Color::get_value() const {
    uint8_t alpha = (uint8_t)(a * 256);
    uint8_t red = (uint8_t)(r * 256);
    uint8_t green = (uint8_t)(g * 256);
    uint8_t blue = (uint8_t)(b * 256);
    uint32_t color = (alpha << 24) | (blue << 16) | (green << 8) | (red);
    return color;
}

UnifiedRender::Color UnifiedRender::Color::lerp(UnifiedRender::Color color1, UnifiedRender::Color color2, float lamda) {
    float r = color1.r * (1 - lamda) + color2.r * lamda;
    float g = color1.g * (1 - lamda) + color2.g * lamda;
    float b = color1.b * (1 - lamda) + color2.b * lamda;
    return UnifiedRender::Color(r, g, b);
}