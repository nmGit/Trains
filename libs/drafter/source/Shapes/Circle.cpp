#include "Drafter/Shapes/Circle.h"

namespace Drafter {

Circle::Circle(Canvas &canvas, config_t config)
    : m_canvas(canvas), m_config(config) {
    canvas.AddItem(*this);
}

Circle::~Circle() {}

void Circle::Draw() {
    BLContext &ctx = m_canvas.GetRenderer();

    ctx.save();

    ctx.set_stroke_style(BLRgba32(0xFFFF6400)); // Orange
    ctx.set_stroke_width(2.0);

    ctx.stroke_circle(BLCircle(
        m_config.position.x,
        m_config.position.y,
        m_config.radius));

    ctx.restore();
}

} // namespace Drafter