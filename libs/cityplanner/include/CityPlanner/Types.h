#pragma once
namespace CityPlanner {

/**
 *
 * @brief The hex_coord_t struct represents the coordinates of a hexagonal tile
 * in the world.
 *
 * https://www.redblobgames.com/grids/hexagons/
 * 
 * The hexagonal grid is represented using axial coordinates (q, r). This struct
 * is used to store the position of a tile in the world and can be used for
 * various calculations related to tile placement and movement.
 *
 * The neighboring tiles can be calculated using the following offsets:
 * - North-East: (q + 1, r)
 * - East: (q + 1, r - 1)
 * - South-East: (q, r - 1)
 * - South-West: (q - 1, r)
 * - West: (q - 1, r + 1)
 * - North-West: (q, r + 1)
 */
struct hex_coord_t {
    int q;
    int r;

    bool operator<(const hex_coord_t &o) const {
        if (q != o.q) return q < o.q;
        return r < o.r;
    }
    bool operator==(const hex_coord_t &o) const { return q == o.q && r == o.r; }
};

struct point_t {
    float x;
    float y;
};
} // namespace CityPlanner