#pragma once

#include <blend2d/blend2d.h>
#include <cstdint>

namespace Drafter {
namespace Color {

/**
 * @brief An RGB color with components in [0, 255].
 */
struct rgb_t {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;
};

/**
 * @brief An HSV color.
 *
 * @p h is in [0, 360), @p s and @p v are in [0, 1].
 */
struct hsv_t {
    float   h = 0.f;   ///< Hue in degrees [0, 360)
    float   s = 0.f;   ///< Saturation [0, 1]
    float   v = 0.f;   ///< Value [0, 1]
    uint8_t a = 255;   ///< Alpha [0, 255]
};

/**
 * @brief Convert an HSV color to RGB.
 *
 * @param hsv Source color in HSV space.
 * @return Equivalent color in RGB space, preserving alpha.
 */
rgb_t HsvToRgb(hsv_t hsv);

/**
 * @brief Convert an RGB color to a packed Blend2D ARGB32 value.
 *
 * @param rgb Source RGB color.
 * @return BLRgba32 suitable for use with BLContext.
 */
BLRgba32 RgbToBL(rgb_t rgb);

/**
 * @brief Convert an HSV color directly to a packed Blend2D ARGB32 value.
 *
 * Convenience wrapper equivalent to RgbToBL(HsvToRgb(hsv)).
 *
 * @param hsv Source color in HSV space.
 * @return BLRgba32 suitable for use with BLContext.
 */
BLRgba32 HsvToBL(hsv_t hsv);

/**
 * @brief Scale the alpha channel of a BLRgba32 color by an opacity factor.
 *
 * @param color   Source color (ARGB32).
 * @param opacity Multiplier in [0, 1]; 1.0 leaves the color unchanged.
 * @return Color with alpha scaled by @p opacity.
 */
BLRgba32 ApplyOpacity(BLRgba32 color, float opacity);

} // namespace Color
} // namespace Drafter
