#include "Drafter/Shapes/HexGrid.h"

#include <cmath>
#include <numbers>

namespace Drafter {

// Flat-top hexagon tiling layout (vertex at 0°, flat edges on top/bottom):
//
//   col_step   = 1.5 × r          (horizontal center-to-center distance)
//   row_step   = r × √3           (vertical center-to-center within a column)
//   odd_offset = row_step × 0.5   (odd columns shift down by half a row)
//
//  Col 0    Col 1    Col 2
//   (0,0)  (1.5r, √3/2·r)  (3r, 0)
//   (0,√3r) (1.5r, 3√3/2·r) ...

HexGrid::HexGrid(config_t config) : m_config(config) {
    BuildPath();
}

HexGrid::~HexGrid() {}

void HexGrid::BuildPath() {
    const float r          = m_config.cell_radius;
    const float col_step   = 1.5f * r;
    const float row_step   = r * std::sqrt(3.0f);
    const float odd_offset = row_step * 0.5f;

    // Add one extra col/row so hexes at the edges are fully visible
    const int num_cols =
        static_cast<int>(std::ceil(m_config.bounds_w / col_step)) + 1;
    const int num_rows =
        static_cast<int>(std::ceil(m_config.bounds_h / row_step)) + 2;

    constexpr float k_two_pi    = 2.0f * std::numbers::pi_v<float>;
    constexpr float k_angle_step = k_two_pi / 6.0f;

    for (int col = 0; col < num_cols; ++col) {
        const float cx     = static_cast<float>(col) * col_step;
        const float y_base = (col % 2 == 1) ? odd_offset : 0.0f;

        for (int row = 0; row < num_rows; ++row) {
            const float cy = y_base + static_cast<float>(row) * row_step;

            m_path.move_to(cx + r * std::cos(0.0f),
                           cy + r * std::sin(0.0f));

            for (int v = 1; v < 6; ++v) {
                const float angle = k_angle_step * static_cast<float>(v);
                m_path.line_to(cx + r * std::cos(angle),
                               cy + r * std::sin(angle));
            }

            m_path.close();
        }
    }
}

void HexGrid::Draw(BLContext &ctx) {
    ctx.save();
    ctx.set_stroke_style(m_config.stroke_color);
    ctx.set_stroke_width(m_config.stroke_width);
    ctx.stroke_path(m_path);
    ctx.restore();
}

} // namespace Drafter