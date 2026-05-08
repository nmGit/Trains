#include "PieMenu.h"

#include "Log/Log.h"

#include <SDL3/SDL.h>
#include <blend2d/blend2d.h>
#include <cmath>
#include <numbers>

namespace Trains {

static constexpr float k_inner_radius = 35.f;
static constexpr float k_outer_radius = 110.f;
static constexpr float k_slice_gap    = 0.06f;
static constexpr float k_two_pi       = 2.f * std::numbers::pi_v<float>;
static constexpr float k_half_pi      = std::numbers::pi_v<float> / 2.f;
static LogContext      logContext("PieMenu");

PieMenu::PieMenu(Drafter::Camera &camera, Drafter::Canvas &canvas)
    : m_camera(camera) {
    Attach(canvas);
}

PieMenu::~PieMenu() = default;

void PieMenu::AddOption(std::string label, std::function<void()> on_select) {
    m_options.push_back({std::move(label), std::move(on_select)});
}

void PieMenu::SetFont(const char *path, float size) {
    m_font = Drafter::Font(path, size);
}

bool PieMenu::Service(const SDL_Event &event) {
    if (!m_open) {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            event.button.button == SDL_BUTTON_RIGHT) {
            m_center_x = event.button.x;
            m_center_y = event.button.y;
            m_mouse_x  = event.button.x;
            m_mouse_y  = event.button.y;
            m_open     = true;
            UpdateHover();
            return true;
        }
        return false;
    }

    switch (event.type) {
    case SDL_EVENT_MOUSE_MOTION:
        m_mouse_x = event.motion.x;
        m_mouse_y = event.motion.y;
        UpdateHover();
        return true;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_RIGHT) {
            if (m_hovered >= 0)
                Log::Info(logContext, "Selected option %d: %s", m_hovered,
                          m_options[m_hovered].label.c_str());
            m_options[m_hovered].on_select();
            Close();
        }
        return true;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_WHEEL:
        return true;

    case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_ESCAPE)
            Close();
        return false;

    default:
        return false;
    }
}

void PieMenu::RenderWorld(BLContext &ctx,
                          const Drafter::draw_params_t & /*params*/) {
    if (!m_open || m_options.empty())
        return;

    const int   n           = static_cast<int>(m_options.size());
    const float slice_angle = k_two_pi / static_cast<float>(n);
    const float cx          = m_center_x;
    const float cy          = m_center_y;

    // Switch to screen space: the canvas has applied the camera transform,
    // so we save/reset to draw in pixel coordinates, then restore.
    ctx.save();
    ctx.reset_transform();

    for (int i = 0; i < n; ++i) {
        const bool hovered = (i == m_hovered);
        // Center each slice on the angle UpdateHover maps to index i.
        const float start = -k_half_pi +
                            (static_cast<float>(i) - 0.5f) * slice_angle +
                            k_slice_gap * 0.5f;
        const float sweep = slice_angle - k_slice_gap;
        const float end   = start + sweep;

        const float cos_s = std::cos(start), sin_s = std::sin(start);
        const float cos_e = std::cos(end), sin_e = std::sin(end);

        BLPath sector;
        sector.move_to(cx + cos_s * k_inner_radius,
                       cy + sin_s * k_inner_radius);
        sector.line_to(cx + cos_s * k_outer_radius,
                       cy + sin_s * k_outer_radius);
        sector.arc_to(cx, cy, k_outer_radius, k_outer_radius, start, sweep);
        sector.line_to(cx + cos_e * k_inner_radius,
                       cy + sin_e * k_inner_radius);
        sector.arc_to(cx, cy, k_inner_radius, k_inner_radius, end, -sweep);
        sector.close();

        ctx.set_fill_style(hovered ? BLRgba32(65, 125, 255, 235)
                                   : BLRgba32(18, 22, 42, 210));
        ctx.fill_path(sector);

        ctx.set_stroke_style(hovered ? BLRgba32(160, 200, 255, 220)
                                     : BLRgba32(90, 105, 155, 140));
        ctx.set_stroke_width(hovered ? 2.0 : 1.0);
        ctx.stroke_path(sector);

        if (m_font.IsValid()) {
            const float mid_a =
                -k_half_pi + static_cast<float>(i) * slice_angle;
            const float label_r = (k_inner_radius + k_outer_radius) * 0.5f;
            const float lx      = cx + std::cos(mid_a) * label_r;
            const float ly      = cy + std::sin(mid_a) * label_r;
            const float tw      = m_font.MeasureWidth(m_options[i].label);
            const float y_baseline =
                ly + (2.f * m_font.Ascent() - m_font.LineHeight()) * 0.5f;
            m_font.DrawText(ctx, lx - tw * 0.5f, y_baseline, m_options[i].label,
                            hovered ? BLRgba32(240, 245, 255, 255)
                                    : BLRgba32(175, 185, 210, 215));
        }
    }

    ctx.restore();
}

void PieMenu::Close() {
    m_open    = false;
    m_hovered = -1;
}

void PieMenu::UpdateHover() {
    const float dx   = m_mouse_x - m_center_x;
    const float dy   = m_mouse_y - m_center_y;
    const float dist = std::sqrt(dx * dx + dy * dy);

    m_hovered = -1;
    if (dist < k_inner_radius || dist > k_outer_radius)
        return;

    const int   n     = static_cast<int>(m_options.size());
    const float slice = k_two_pi / static_cast<float>(n);
    const float angle = std::atan2(dy, dx);
    // Rotate so slice 0 is centred at the top (−π/2) and spans ±slice/2.
    const float norm =
        std::fmod(angle + k_half_pi + slice * 0.5f + k_two_pi * 2.f, k_two_pi);
    m_hovered = static_cast<int>(norm / slice);
    if (m_hovered >= n)
        m_hovered = n - 1;
}

} // namespace Trains
