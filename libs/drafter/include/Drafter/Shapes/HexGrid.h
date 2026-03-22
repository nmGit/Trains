#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

#include <blend2d/blend2d.h>

namespace Drafter {

/**
 * @brief Draws a flat-top hexagonal tile grid over a given area.
 *
 * Tile centers are precomputed at construction time, and the entire grid is
 * batched into a single BLPath so Draw() costs one stroke call per frame.
 */
class HexGrid : public Shape {
  public:
    struct config_t {
        float    cell_radius  = 25.f;   ///< Circumradius of each hexagon
        float    bounds_w     = 480.f;  ///< Width of the area to tile
        float    bounds_h     = 480.f;  ///< Height of the area to tile
        BLRgba32 stroke_color{0xFF3A3A5C};
        float    stroke_width = 1.0f;
    };

    explicit HexGrid(config_t config);
    ~HexGrid();

    void Draw(BLContext &ctx) override;

  private:
    /// Builds m_path from config. Called once at construction.
    void BuildPath();

    config_t m_config;
    BLPath   m_path; ///< All hexagons batched into one path for efficiency
};

} // namespace Drafter