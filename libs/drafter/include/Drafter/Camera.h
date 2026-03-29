#pragma once

#include "Types.h"

#include <SDL3/SDL.h>
#include <optional>

// Forward declarations
class BLContext;

namespace Drafter {

class Canvas;

/**
 * @brief A 2D camera providing pan and zoom for a Canvas.
 *
 * When attached to a Canvas, the camera subscribes to SDL events and updates
 * its transform each frame.  Canvas applies the transform to its BLContext
 * before drawing shapes, so shapes always work in world coordinates.
 *
 * Pan is driven by WASD / arrow keys (with acceleration and friction) and by
 * mouse drag.  Zoom is driven by scroll wheel / touchpad gestures and
 * interpolated in log-space for smooth response.
 *
 * If no camera is attached to a Canvas, the canvas draws with the identity
 * transform (world coordinates = screen pixels, origin at top-left).
 */
class Camera {
  public:
    Camera();

    /**
     * @brief Attach this camera to a canvas.
     *
     * Subscribes to the canvas's SDL event stream so the camera can react to
     * keyboard, mouse, and scroll input.
     *
     * @param canvas The canvas to attach to.
     */
    void Attach(Canvas &canvas);

    /**
     * @brief Detach from the currently attached canvas.
     *
     * After detaching, the canvas reverts to the identity transform.
     */
    void Detach();

    /**
     * @brief Advance camera state by one frame.
     *
     * Reads keyboard state for pan input, applies acceleration / friction,
     * and interpolates zoom toward its target.  Called by Canvas each frame
     * before drawing.
     */
    void Update();

    /**
     * @brief Apply the camera transform to a BLContext.
     *
     * Translates to screen center, scales by zoom, then offsets by camera
     * position.  The caller is responsible for calling ctx.save() before and
     * ctx.restore() after.
     *
     * @param ctx The Blend2D context to transform.
     * @param geo The canvas geometry (pixel dimensions).
     */
    void ApplyTransform(BLContext &ctx, const geometry_t &geo) const;

    /**
     * @brief Returns the visible world-space rectangle.
     *
     * @param geo The canvas geometry (pixel dimensions).
     * @return A bounds_t describing the world-space area currently on screen.
     */
    bounds_t GetViewBounds(const geometry_t &geo) const;

    /**
     * @brief Sets world-space bounds that the view frustum must not exceed.
     *
     * When set, the camera clamps its position and minimum zoom so that
     * GetViewBounds() always stays within these bounds.
     *
     * @param world_bounds The world-space rectangle to constrain to.
     */
    void SetBounds(bounds_t world_bounds);

    /**
     * @brief Returns the current zoom level.
     *
     * @return Zoom factor; 1.0 is no zoom, >1 is zoomed in, <1 is zoomed out.
     */
    float GetZoom() const {
        return m_zoom;
    }

  private:
    /** @brief Handles an SDL event forwarded by the attached Canvas. */
    void HandleSDLEvent(const SDL_Event &event);

    Canvas                  *m_canvas = nullptr;
    std::optional<bounds_t>  m_world_bounds;

    // --- Pan state ---
    float m_camera_x = 0.f;
    float m_camera_y = 0.f;
    float m_pan_vx   = 0.f;
    float m_pan_vy   = 0.f;

    // --- Zoom state ---
    float m_log_zoom        = 0.f; ///< Current log-zoom (0 = zoom 1.0)
    float m_log_zoom_target = 0.f; ///< Target log-zoom (smoothly interpolated)
    float m_zoom            = 1.f; ///< Derived: exp(m_log_zoom)

    // --- Mouse drag state ---
    bool  m_dragging     = false;
    float m_last_mouse_x = 0.f;
    float m_last_mouse_y = 0.f;
    float m_drag_vx =
        0.f; ///< EMA velocity tracked during drag (screen px/frame)
    float m_drag_vy = 0.f;

    // --- Scroll pan state ---
    float m_scroll_vx = 0.f; ///< EMA velocity tracked during scroll gesture
    float m_scroll_vy = 0.f;
    bool  m_had_scroll_this_frame = false;
    bool  m_was_scrolling         = false;

    // --- Tuning constants ---
    static constexpr float k_pan_accel     = 0.8f;
    static constexpr float k_pan_max_speed = 20.f;
    static constexpr float k_pan_friction =
        0.95f; ///< Numerically larger -> less friction.
    static constexpr float k_zoom_min      = 0.1f;
    static constexpr float k_zoom_max      = 5.0f;
    static constexpr float k_zoom_sensitivity =
        0.5f; ///< Numerically larger -> more zoom per scroll delta.
    static constexpr float k_zoom_smoothing =
        0.7f; ///< Numerically larger -> coarser but less lag.
    static constexpr float k_scroll_pan_sensitivity =
        50.0f; ///< scroll delta → world units
    static constexpr float k_flick_smoothing =
        0.0f; ///< EMA alpha for flick velocity
    static constexpr float k_flick_min_speed =
        0.3f; ///< minimum speed to trigger momentum
};

} // namespace Drafter
