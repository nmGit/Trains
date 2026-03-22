#pragma once

#include "CityPlanner/City.h"
#include "Drafter/Canvas.h"

#include <blend2d/blend2d.h>

namespace Trains {

/**
 * @brief Visualizes a single city on the world hex grid.
 *
 * Each frame, Service() fills all claimed tiles with a semi-transparent color
 * and strokes only the outer boundary edges (edges whose neighbor is not part
 * of the same city), so the border does not cut between two tiles that belong
 * to the same city.
 *
 * A random hue is assigned at construction; the fill is a darker, slightly
 * transparent variant of that hue, and the border is a thicker, saturated
 * variant.
 */
class CityView {
  public:
    CityView(CityPlanner::City *city, Drafter::Canvas &canvas, float cell_radius);

    void Service();

  private:
    /// Returns the pixel-space center of a tile (scaled by m_cell_radius).
    BLPoint TileCenter(CityPlanner::hex_coord_t tile) const;

    CityPlanner::City *m_city;
    Drafter::Canvas   &m_canvas;
    float              m_cell_radius;

    BLRgba32 m_fill_color;
    BLRgba32 m_border_color;
};

} // namespace Trains
