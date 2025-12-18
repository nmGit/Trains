#pragma once

#include "Events/Event.h"
#include "Types.h"
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

    /**
     * @brief Resizes the canvas to the specified size.
     *
     * @param size The new size of the canvas.
     */
    void Resize(rect_t size);

    /**
     * @brief Gets the size of the shape.
     *
     * @return The current size of the shape.
     */
    rect_t Size();

    /**
     * @brief Repositions the shape to the specified position.
     *
     * @param position The new position of the shape.
     */
    void Reposition(point_t position);

    /**
     * @brief Gets the current size of the shape.
     *
     * @return The current size of the shape.
     */
    point_t Position();

    virtual void Draw() = 0;
    /**
     * @name Events
     * @{
     */
    /**
     * @brief Defines a type alias for an event that is triggered on resize
     * operations.
     */
    typedef Event<void, Shape &, rect_t> resize_event_t;

    typedef Event<void, Shape &> draw_event_t;

    typedef Event<void, Shape &> reposition_event_t;

    /**
     * @brief Resize event.
     *
     * Subscribe to this resize event to be notified when the canvas size
     * changes.
     *
     * @return A reference to the resize event.
     */
    resize_event_t &OnResize();

    draw_event_t &onDraw();
    /// @} // Events

  private:
    // Shape properties
    geometry_t m_geometry;

    // Events
    resize_event_t m_resize_event;
    draw_event_t   m_draw_event;
    draw_event_t   m_reposition_event;
};

} // namespace Drafter