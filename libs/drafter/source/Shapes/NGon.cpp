#include "Drafter/Shapes/NGon.h"

#include <cmath>

namespace Drafter {

NGon::NGon(Canvas &canvas, config_t config)
    : m_config(config), m_canvas(canvas) {
    canvas.AddItem(*this);
}

NGon::~NGon() {}

void NGon::Draw() {
    if (m_config.sides < 3) {
        return;
    }

    BLContext &ctx = m_canvas.GetRenderer();

    BLPath path;

    const float angle_step = 2.0f * SDL_PI_F / static_cast<float>(m_config.sides);

    // Move to the first vertex
    path.move_to(
        m_config.position.x + m_config.radius * std::cos(0.0f),
        m_config.position.y + m_config.radius * std::sin(0.0f));

    // Line to each subsequent vertex
    for (size_t i = 1; i < m_config.sides; i++) {
        const float angle = angle_step * static_cast<float>(i);
        path.line_to(
            m_config.position.x + m_config.radius * std::cos(angle),
            m_config.position.y + m_config.radius * std::sin(angle));
    }

    path.close();

    ctx.save();

    ctx.set_stroke_style(BLRgba32(0xFFFF6400)); // Orange
    ctx.set_stroke_width(2.0);
    ctx.stroke_path(path);

    ctx.restore();
}

} // namespace Drafter