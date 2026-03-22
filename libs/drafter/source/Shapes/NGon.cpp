#include "Drafter/Shapes/NGon.h"

#include <blend2d/blend2d.h>
#include <cmath>
#include <numbers>

namespace Drafter {

NGon::NGon(config_t config) : m_config(config) {}

NGon::~NGon() {}

void NGon::Draw(BLContext &ctx) {
    if (m_config.sides < 3) {
        return;
    }

    const float angle_step =
        2.0f * std::numbers::pi_v<float> / static_cast<float>(m_config.sides);

    BLPath path;

    path.move_to(m_config.position.x + m_config.radius * std::cos(0.0f),
                 m_config.position.y + m_config.radius * std::sin(0.0f));

    for (size_t i = 1; i < m_config.sides; i++) {
        const float angle = angle_step * static_cast<float>(i);
        path.line_to(m_config.position.x + m_config.radius * std::cos(angle),
                     m_config.position.y + m_config.radius * std::sin(angle));
    }

    path.close();

    ctx.save();
    ctx.set_stroke_style(BLRgba32(0xFFFF6400));
    ctx.set_stroke_width(2.0);
    ctx.stroke_path(path);
    ctx.restore();
}

} // namespace Drafter