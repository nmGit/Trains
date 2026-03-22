#include "CityPlanner/Utils.h"
#include <cmath>

namespace CityPlanner {

std::array<hex_coord_t, 6> Neighbors(hex_coord_t h) {
    return {{
        {h.q + 1, h.r    },
        {h.q - 1, h.r    },
        {h.q,     h.r + 1},
        {h.q,     h.r - 1},
        {h.q + 1, h.r - 1},
        {h.q - 1, h.r + 1},
    }};
}

point_t AxialToPixel(const hex_coord_t &hex) {
    // Assuming a hexagon size of 1 unit for simplicity
    const float hex_size = 1.0f;
    point_t     pixel;
    pixel.x = hex_size * (3.0f / 2.0f * hex.q);
    pixel.y = hex_size * (sqrt(3.0f) / 2.0f * hex.q + sqrt(3.0f) * hex.r);
    return pixel;
}

} // namespace CityPlanner
