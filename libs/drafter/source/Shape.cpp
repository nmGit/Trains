#include "Drafter/Shape.h"

namespace Drafter {
Shape::Shape() {}

Shape::~Shape() {}

void Shape::Resize(rect_t size) {
    m_geometry.size = size;
    m_resize_event.Emit(*this, m_geometry.size);
}

rect_t Shape::Size() {
    return m_geometry.size;
}

void Shape::Reposition(point_t position) {
    m_geometry.pos = position;
    m_reposition_event.Emit(*this);
};

point_t Shape::Position() {
    return m_geometry.pos;
}

Shape::draw_event_t &Shape::onDraw() {
    return m_draw_event;
}

void Shape::Draw() {
    m_draw_event.Emit(*this);
}

} // namespace Drafter