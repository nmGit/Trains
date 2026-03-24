#include "Drafter/Camera.h"
#include "Drafter/Canvas.h"

#include <algorithm>
#include <blend2d/blend2d.h>
#include <cmath>

namespace Drafter {

Camera::Camera() {}

void Camera::Attach(Canvas &canvas) {
    if (m_canvas) {
        Detach();
    }
    m_canvas = &canvas;
    m_canvas->OnSDLEvent().Subscribe(this, &Camera::HandleSDLEvent);
    m_canvas->SetCamera(this);
}

void Camera::Detach() {
    if (!m_canvas) return;
    m_canvas->SetCamera(nullptr);
    m_canvas = nullptr;
}

void Camera::Update() {
    // --- Scroll momentum handoff ---
    // The first frame after scroll events stop, transfer the EMA velocity to
    // the pan velocity so the friction model carries the map forward.
    if (!m_had_scroll_this_frame && m_was_scrolling) {
        const float speed = std::sqrt(m_scroll_vx * m_scroll_vx +
                                      m_scroll_vy * m_scroll_vy);
        if (speed > k_flick_min_speed) {
            m_pan_vx = m_scroll_vx;
            m_pan_vy = m_scroll_vy;
        }
        m_scroll_vx = 0.f;
        m_scroll_vy = 0.f;
    }
    m_was_scrolling         = m_had_scroll_this_frame;
    m_had_scroll_this_frame = false;

    // --- Keyboard pan and friction ---
    // Skipped while dragging: position is updated directly in the event
    // handler, so applying velocity here would cause double-movement.
    if (!m_dragging) {
        const bool *keys = SDL_GetKeyboardState(nullptr);

        float ax = 0.f, ay = 0.f;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  ax -= 1.f;
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) ax += 1.f;
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    ay -= 1.f;
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  ay += 1.f;

        if (ax != 0.f || ay != 0.f) {
            const float len = std::sqrt(ax * ax + ay * ay);
            ax /= len;
            ay /= len;
            m_pan_vx += ax * k_pan_accel;
            m_pan_vy += ay * k_pan_accel;
            const float speed = std::sqrt(m_pan_vx * m_pan_vx +
                                          m_pan_vy * m_pan_vy);
            if (speed > k_pan_max_speed) {
                m_pan_vx = m_pan_vx / speed * k_pan_max_speed;
                m_pan_vy = m_pan_vy / speed * k_pan_max_speed;
            }
        } else {
            m_pan_vx *= k_pan_friction;
            m_pan_vy *= k_pan_friction;
        }

        m_camera_x += m_pan_vx / m_zoom;
        m_camera_y += m_pan_vy / m_zoom;
    }

    // --- Smooth zoom ---
    m_log_zoom += (m_log_zoom_target - m_log_zoom) * k_zoom_smoothing;
    m_zoom = std::exp(m_log_zoom);
    m_zoom = std::clamp(m_zoom, k_zoom_min, k_zoom_max);
}

void Camera::ApplyTransform(BLContext &ctx, const geometry_t &geo) const {
    ctx.translate(geo.size.w * 0.5, geo.size.h * 0.5);
    ctx.scale(static_cast<double>(m_zoom));
    ctx.translate(static_cast<double>(-m_camera_x),
                  static_cast<double>(-m_camera_y));
}

bounds_t Camera::GetViewBounds(const geometry_t &geo) const {
    const float half_w = (geo.size.w * 0.5f) / m_zoom;
    const float half_h = (geo.size.h * 0.5f) / m_zoom;
    return {
        .x = m_camera_x - half_w,
        .y = m_camera_y - half_h,
        .w = half_w * 2.0f,
        .h = half_h * 2.0f,
    };
}

void Camera::HandleSDLEvent(const SDL_Event &event) {
    switch (event.type) {

    case SDL_EVENT_MOUSE_WHEEL: {
        const bool ctrl = SDL_GetModState() & SDL_KMOD_CTRL;
        if (ctrl) {
            // Pinch gesture (or Ctrl+scroll) → zoom
            m_log_zoom_target += event.wheel.y * k_zoom_sensitivity;
            m_log_zoom_target = std::clamp(m_log_zoom_target,
                                           std::log(k_zoom_min),
                                           std::log(k_zoom_max));
        } else {
            // Two-finger pan: update position directly (1:1 tracking) and
            // track EMA velocity for momentum on gesture end.
            // Cancel any existing keyboard momentum when the gesture starts.
            if (!m_was_scrolling) {
                m_pan_vx = 0.f;
                m_pan_vy = 0.f;
            }

            const float dx =  event.wheel.x * k_scroll_pan_sensitivity / m_zoom;
            const float dy = -event.wheel.y * k_scroll_pan_sensitivity / m_zoom;
            m_camera_x += dx;
            m_camera_y += dy;

            // EMA in screen-space units so the momentum scale matches keyboard pan.
            const float vx = event.wheel.x * k_scroll_pan_sensitivity;
            const float vy = -event.wheel.y * k_scroll_pan_sensitivity;
            m_scroll_vx = m_scroll_vx * (1.f - k_flick_smoothing) +
                          vx           *        k_flick_smoothing;
            m_scroll_vy = m_scroll_vy * (1.f - k_flick_smoothing) +
                          vy           *        k_flick_smoothing;

            m_had_scroll_this_frame = true;
        }
        break;
    }

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            m_dragging     = true;
            m_last_mouse_x = event.button.x;
            m_last_mouse_y = event.button.y;
            m_drag_vx      = 0.f;
            m_drag_vy      = 0.f;
            // Cancel existing momentum so the drag takes full control.
            m_pan_vx = 0.f;
            m_pan_vy = 0.f;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT && m_dragging) {
            m_dragging = false;
            // Transfer flick velocity to pan so the friction model carries it.
            const float speed = std::sqrt(m_drag_vx * m_drag_vx +
                                          m_drag_vy * m_drag_vy);
            if (speed > k_flick_min_speed) {
                m_pan_vx = m_drag_vx;
                m_pan_vy = m_drag_vy;
            }
            m_drag_vx = 0.f;
            m_drag_vy = 0.f;
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (m_dragging) {
            // Direct 1:1 position tracking.
            const float dx = -(event.motion.x - m_last_mouse_x);
            const float dy = -(event.motion.y - m_last_mouse_y);
            m_camera_x += dx / m_zoom;
            m_camera_y += dy / m_zoom;
            m_last_mouse_x = event.motion.x;
            m_last_mouse_y = event.motion.y;

            // EMA velocity in screen-space units for flick on release.
            m_drag_vx = m_drag_vx * (1.f - k_flick_smoothing) +
                        dx         *        k_flick_smoothing;
            m_drag_vy = m_drag_vy * (1.f - k_flick_smoothing) +
                        dy         *        k_flick_smoothing;
        }
        break;

    default:
        break;
    }
}

} // namespace Drafter
