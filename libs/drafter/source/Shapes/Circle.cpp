#include "Drafter/Shapes/Circle.h"

#include <blend2d/blend2d.h>

namespace Drafter {

Circle::Circle(config_t config) : m_config(config) {}

Circle::~Circle() {}

void Circle::Draw(BLContext &ctx) {
    ctx.save();

    ctx.set_stroke_style(BLRgba32(0xFFFF6400));
    ctx.set_stroke_width(2.0);
    ctx.stroke_circle(BLCircle(m_config.position.x,
                               m_config.position.y,
                               m_config.radius));

    ctx.restore();
}

} // namespace Drafter