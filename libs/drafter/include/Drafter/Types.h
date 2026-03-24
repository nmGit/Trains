#pragma once

#include <algorithm>

namespace Drafter {
/**
 * @brief Represents a rectangle with width and height.
 */
struct rect_t {
    float w;
    float h;
};

struct point_t {
    float x;
    float y;
};

struct geometry_t {
    rect_t  size;
    point_t pos;
};

/**
 * @brief An axis-aligned bounding rectangle in world space.
 *
 * Passed to Shape::Draw() so shapes can cull geometry that falls outside
 * the visible area. (x, y) is the top-left corner; w and h are the extents.
 */
struct bounds_t {
    float x = 0.f;
    float y = 0.f;
    float w = 0.f;
    float h = 0.f;

    /** @brief Returns true if the circle at (cx, cy) with given radius overlaps this bounds. */
    bool Intersects(float cx, float cy, float radius) const {
        return cx + radius >= x && cx - radius <= x + w &&
               cy + radius >= y && cy - radius <= y + h;
    }
};

/**
 * @brief Defines a zoom range over which a stroke or fill fades to zero opacity.
 *
 * Opacity is 1 on the @p fade_start side and 0 at @p fade_end, interpolating
 * linearly between the two.  When both values are 0 the fade is disabled and
 * the shape always draws at full opacity.
 *
 * Either direction is supported:
 *   - @p fade_start > @p fade_end : fades out as zoom *decreases* (disappears when small)
 *   - @p fade_start < @p fade_end : fades out as zoom *increases* (disappears when large)
 */
struct zoom_fade_t {
    float fade_start = 0.f;  ///< Zoom level at which opacity begins to drop
    float fade_end   = 0.f;  ///< Zoom level at which opacity reaches 0 (culling threshold)

    /**
     * @brief Returns the opacity in [0, 1] for the given zoom level.
     *
     * @param zoom Current camera zoom.
     * @return 1.0 when fully visible, 0.0 when culled.
     */
    float Opacity(float zoom) const {
        if (fade_start == 0.f && fade_end == 0.f) return 1.f;
        if (fade_start == fade_end) return zoom >= fade_start ? 1.f : 0.f;
        return std::clamp((zoom - fade_end) / (fade_start - fade_end), 0.f, 1.f);
    }

    /**
     * @brief Returns true if geometry should be skipped entirely at this zoom level.
     *
     * @param zoom Current camera zoom.
     */
    bool ShouldSkip(float zoom) const { return Opacity(zoom) == 0.f; }
};

/**
 * @brief Parameters passed to Shape::Draw() each frame.
 *
 * Bundles the visible world-space rectangle and the current camera zoom so
 * shapes can perform both view frustum culling and zoom-based opacity fading
 * without needing a reference to the camera.
 */
struct draw_params_t {
    bounds_t view_bounds;    ///< Visible area in world space
    float    zoom = 1.f;     ///< Current camera zoom (1 = no zoom)
};

} // namespace Drafter