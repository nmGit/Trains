#pragma once

#include "CityPlanner/World.h"
#include "Drafter/Drafter.h"

/**
 * @brief The WorldView class is responsible for visualizing the world.
 */
class WorldView {
  public:
    /**
     * @brief Constructs a new WorldView object.
     *
     * @param world  Reference to the CityPlanner::World object to visualize.
     * @param canvas Reference to the Drafter::Canvas object to draw on.
     */
    WorldView(CityPlanner::World &world, Drafter::Canvas &canvas);

    /**
     * @brief Draws the current state of the world onto the canvas.
     */
    void Draw();

  protected:

  private:
    CityPlanner::World &m_world;
    Drafter::Canvas    &m_canvas;
    LogContext          m_log_context{"WorldView"};
};
