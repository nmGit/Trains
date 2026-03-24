#include "Drafter/Shapes/HexGrid.h"
#include "Drafter/Utils/Color.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace Drafter {

HexGrid::HexGrid(config_t config) : m_config(config) {
    m_col_step = 1.5f * config.cell_radius;
    m_row_step = config.cell_radius * std::sqrt(3.0f);

    // Precompute the 6 vertex offsets shared by every hexagon.
    constexpr float k_angle_step = 2.0f * std::numbers::pi_v<float> / 6.0f;
    const float r = config.cell_radius;
    for (int v = 0; v < 6; ++v) {
        const float a = k_angle_step * static_cast<float>(v);
        m_vx[v] = r * std::cos(a);
        m_vy[v] = r * std::sin(a);
    }
}

HexGrid::~HexGrid() {}

void HexGrid::SetPosition(point_t position) {
    m_config.position = position;
    m_path_dirty      = true;
}

void HexGrid::BuildPath(const bounds_t &view_bounds) {
    const float ox         = m_config.position.x;
    const float oy         = m_config.position.y;
    const float odd_offset = m_row_step * 0.5f;

    // Visible column range relative to the grid origin.
    // No upper clamp — WorldView positions the grid so it always covers
    // the viewport, and col_min is always >= 0.
    const int col_min = std::max(0,
        static_cast<int>(std::floor((view_bounds.x - ox) / m_col_step)) - 1);
    const int col_max =
        static_cast<int>(std::ceil((view_bounds.x + view_bounds.w - ox) / m_col_step)) + 1;

    m_path.clear();

    for (int col = col_min; col <= col_max; ++col) {
        const float cx     = ox + static_cast<float>(col) * m_col_step;
        const float y_base = oy + (col % 2 != 0 ? odd_offset : 0.0f);

        // Visible row range for this column.
        const int row_min = std::max(0,
            static_cast<int>(std::floor((view_bounds.y - y_base) / m_row_step)) - 1);
        const int row_max =
            static_cast<int>(std::ceil((view_bounds.y + view_bounds.h - y_base) / m_row_step)) + 1;

        for (int row = row_min; row <= row_max; ++row) {
            const float cy = y_base + static_cast<float>(row) * m_row_step;

            m_path.move_to(cx + m_vx[0], cy + m_vy[0]);
            for (int v = 1; v < 6; ++v) {
                m_path.line_to(cx + m_vx[v], cy + m_vy[v]);
            }
            m_path.close();
        }
    }
}

void HexGrid::Draw(BLContext &ctx, draw_params_t params) {
    const float opacity = m_config.stroke_fade.Opacity(params.zoom);
    if (opacity == 0.f) return;

    const bounds_t &vb  = params.view_bounds;
    const point_t  &pos = m_config.position;

    if (m_path_dirty             ||
        vb.x  != m_last_bounds.x ||
        vb.y  != m_last_bounds.y ||
        vb.w  != m_last_bounds.w ||
        vb.h  != m_last_bounds.h ||
        pos.x != m_last_position.x ||
        pos.y != m_last_position.y)
    {
        BuildPath(vb);
        m_last_bounds   = vb;
        m_last_position = pos;
        m_path_dirty    = false;
    }

    ctx.save();
    ctx.set_stroke_style(Color::ApplyOpacity(m_config.stroke_color, opacity));
    ctx.set_stroke_width(m_config.stroke_width);
    ctx.stroke_path(m_path);
    ctx.restore();
}

} // namespace Drafter
