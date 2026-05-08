#pragma once
#include "Drafter/Drafter.h"
#include "PieMenu.h"

namespace Trains {

/**
 * @brief Top-level UI coordinator housing the pie menu and interface camera.
 */
class Interface {
  public:
    /**
     * @brief Construct and attach all interface overlays to the canvas.
     *
     * @param canvas The canvas to attach to.
     */
    Interface(Drafter::Canvas &canvas);
    ~Interface();

    /** @brief Perform any post-construction initialisation. */
    void Start();

    /** @brief Returns the pie menu for option registration and font setup. */
    PieMenu &GetPieMenu() { return m_pie_menu; }

  private:
    Drafter::Camera  m_camera;
    Drafter::Canvas &m_canvas;
    PieMenu          m_pie_menu{m_camera, m_canvas};
};

} // namespace Trains
