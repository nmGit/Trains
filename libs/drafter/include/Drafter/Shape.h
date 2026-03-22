#pragma once

#include "Events/Event.h"
#include "Types.h"

// Forward declaration — avoids pulling the full blend2d header into every TU
class BLContext;

namespace Drafter {

/**
 * @brief The Shape class represents a drawable shape in the Drafter library.
 *
 * This class serves as a base for various shapes that can be drawn on the
 * Drafter::Canvas.
 */
class Shape {
  public:
    Shape();
    ~Shape();

    void    Resize(rect_t size);
    rect_t  Size();
    void    Reposition(point_t position);
    point_t Position();

    /**
     * @brief Draw the shape onto the provided Blend2D context.
     *
     * @param ctx The active BLContext to draw into.
     */
    virtual void Draw(BLContext &ctx) = 0;

    geometry_t     &GetGeometry();
    /**
     * @name Events
     * @{
     */
    typedef Event<void, Shape &, rect_t> resize_event_t;
    typedef Event<void, Shape &>         draw_event_t;
    typedef Event<void, Shape &>         reposition_event_t;

    resize_event_t &OnResize();
    draw_event_t   &onDraw();
    /// @} // Events

  private:
    geometry_t     m_geometry;
    resize_event_t m_resize_event;
    draw_event_t   m_draw_event;
    draw_event_t   m_reposition_event;
};

} // namespace Drafter