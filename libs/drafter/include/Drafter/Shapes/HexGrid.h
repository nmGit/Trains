#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

#include <blend2d/blend2d.h>

namespace Drafter {

/**
 * @brief Draws a flat-top hexagonal tile grid covering a world-space area.
 *
 * The grid originates at @p config.position in world space.  WorldView is
 * responsible for snapping this position to the nearest even-column boundary
 * each frame so the pattern appears globally aligned as the camera pans.
 *
 * The path is only rebuilt when the visible bounds or position change;
 * when the camera is stationary the cached path is reused at zero CPU cost.
 *
 * Vertex offsets for the shared hex shape are precomputed at construction so
 * no trigonometry is performed inside the per-frame draw path.
 */
class HexGrid : public Shape {
  public:
    struct config_t {
        point_t     position;                   ///< World-space origin of column 0, row 0
        float       cell_radius  = 25.f;        ///< Circumradius of each hexagon
        BLRgba32    stroke_color{0xFF3A3A5C};
        float       stroke_width = 1.0f;
        zoom_fade_t stroke_fade;                ///< Zoom range over which stroke fades out
    };

    explicit HexGrid(config_t config);
    ~HexGrid();

    /**
     * @brief Updates the world-space origin of the grid.
     *
     * Marks the cached path dirty so it is rebuilt on the next Draw() call.
     * WorldView calls this each frame to keep the grid aligned under the camera.
     *
     * @param position New world-space origin.
     */
    void SetPosition(point_t position);

    void Draw(BLContext &ctx, draw_params_t params) override;

  private:
    /**
     * @brief Rebuilds m_path to contain only hexagons that intersect view_bounds.
     *
     * Uses precomputed vertex offsets and the grid's world-space position.
     * Column parity (odd/even stagger) is based on the local column index;
     * WorldView ensures the position is always snapped to an even global column
     * so local and global parity agree.
     *
     * @param view_bounds Visible world-space rectangle used for culling.
     */
    void BuildPath(const bounds_t &view_bounds);

    config_t m_config;
    BLPath   m_path;         ///< Cached path — rebuilt only when view or position changes

    // Precomputed geometry (set at construction, never change)
    float m_col_step = 0.f;  ///< Horizontal distance between column centres
    float m_row_step = 0.f;  ///< Vertical distance between row centres
    float m_vx[6]    = {};   ///< Precomputed x offsets of the 6 hex vertices
    float m_vy[6]    = {};   ///< Precomputed y offsets of the 6 hex vertices

    bounds_t m_last_bounds{};    ///< View bounds used for the cached path
    point_t  m_last_position{};  ///< Position used for the cached path
    bool     m_path_dirty = true;
};

} // namespace Drafter
