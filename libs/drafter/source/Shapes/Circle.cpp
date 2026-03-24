#include "Drafter/Shapes/Circle.h"
#include "Drafter/Utils/Color.h"

#include <blend2d/blend2d.h>

namespace Drafter {

Circle::Circle(config_t config) : m_config(config) {}

Circle::~Circle() {}

void Circle::Draw(BLContext &ctx, draw_params_t params) {
    const float stroke_opacity = m_config.stroke_fade.Opacity(params.zoom);
    if (stroke_opacity == 0.f) return;

    ctx.save();
    ctx.set_stroke_style(Color::ApplyOpacity(BLRgba32(0xFFFF6400), stroke_opacity));
    ctx.set_stroke_width(2.0);
    ctx.stroke_circle(BLCircle(m_config.position.x,
                               m_config.position.y,
                               m_config.radius));
    ctx.restore();
}

} // namespace Drafter