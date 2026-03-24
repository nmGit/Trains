#pragma once

#include "CityView.h"
#include "CityPlanner/World.h"
#include "Drafter/Camera.h"
#include "Drafter/Drafter.h"
#include "Drafter/Shapes/HexGrid.h"

#include <memory>
#include <optional>
#include <vector>

namespace Trains {
/**
 * @brief The WorldView class is responsible for visualizing the world.
 *
 * It uses the Drafter::Canvas to render the current state of the
 * CityPlanner::World.  Pan and zoom are handled by a Drafter::Camera
 * attached to the canvas.
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
    void Service();

    /**
     * @brief Starts the WorldView, initializing necessary components.
     */
    void Start();

  protected:
    void SlotRegionAdded(CityPlanner::Region &region);
    void SlotCityAdded(CityPlanner::City &city);

  private:
    static constexpr float k_cell_radius = 10.f;

    Drafter::Canvas    &m_canvas;
    CityPlanner::World &m_world;
    Drafter::Camera     m_camera;

    std::optional<Drafter::HexGrid>        m_hex_grid;
    std::vector<std::unique_ptr<CityView>> m_city_views;

    LogContext m_log_context{"WorldView"};
};
} // namespace Trains
