#include "Drafter/Utils/Color.h"

#include <cmath>

namespace Drafter {
namespace Color {

rgb_t HsvToRgb(hsv_t hsv) {
    const float h = hsv.h;
    const float s = hsv.s;
    const float v = hsv.v;

    const float c = v * s;
    const float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    const float m = v - c;

    float r, g, b;
    if      (h < 60.f)  { r = c; g = x; b = 0; }
    else if (h < 120.f) { r = x; g = c; b = 0; }
    else if (h < 180.f) { r = 0; g = c; b = x; }
    else if (h < 240.f) { r = 0; g = x; b = c; }
    else if (h < 300.f) { r = x; g = 0; b = c; }
    else                { r = c; g = 0; b = x; }

    return {
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255),
        hsv.a,
    };
}

BLRgba32 RgbToBL(rgb_t rgb) {
    return BLRgba32(rgb.r, rgb.g, rgb.b, rgb.a);
}

BLRgba32 HsvToBL(hsv_t hsv) {
    return RgbToBL(HsvToRgb(hsv));
}

BLRgba32 ApplyOpacity(BLRgba32 color, float opacity) {
    const uint32_t original_a = (color.value >> 24) & 0xFF;
    const uint32_t scaled_a   = static_cast<uint32_t>(original_a * opacity);
    return BLRgba32((color.value & 0x00FFFFFFu) | (scaled_a << 24));
}

} // namespace Color
} // namespace Drafter
