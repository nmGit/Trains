#include "Types/Utils.h"
#include "Types/Types.h"
#include <cmath>

namespace Types {

std::array<Types::hex_coord_t, 6> Neighbors(Types::hex_coord_t h) {
    return {{
        {h.q + 1, h.r    },
        {h.q - 1, h.r    },
        {h.q,     h.r + 1},
        {h.q,     h.r - 1},
        {h.q + 1, h.r - 1},
        {h.q - 1, h.r + 1},
    }};
}

Types::hex_coord_t PixelToAxial(Types::point_t pixel) {
    // Inverse of AxialToPixel (flat-top layout, hex_size = 1):
    //   fq =  2/3 * px
    //   fr = -1/3 * px + 1/sqrt(3) * py
    const float fq = ( 2.f / 3.f) * pixel.x;
    const float fr = (-1.f / 3.f) * pixel.x + (1.f / std::sqrt(3.f)) * pixel.y;

    // Convert fractional axial to cube coords and round to nearest hex.
    const float fx = fq;
    const float fz = fr;
    const float fy = -fq - fr;

    int rx = static_cast<int>(std::round(fx));
    int ry = static_cast<int>(std::round(fy));
    int rz = static_cast<int>(std::round(fz));

    // Restore the cube-coordinate constraint (x + y + z == 0) by adjusting
    // the component with the largest rounding error.
    const float dx = std::abs(static_cast<float>(rx) - fx);
    const float dy = std::abs(static_cast<float>(ry) - fy);
    const float dz = std::abs(static_cast<float>(rz) - fz);

    if (dx > dy && dx > dz)
        rx = -ry - rz;
    else if (dy > dz)
        ry = -rx - rz;
    else
        rz = -rx - ry;

    return {rx, rz}; // axial: q = cube x, r = cube z
}

Types::point_t AxialToPixel(const Types::hex_coord_t &hex) {
    // Assuming a hexagon size of 1 unit for simplicity
    const float hex_size = 1.0f;
    Types::point_t     pixel;
    pixel.x = hex_size * (3.0f / 2.0f * hex.q);
    pixel.y = hex_size * (sqrt(3.0f) / 2.0f * hex.q + sqrt(3.0f) * hex.r);
    return pixel;
}

int HexDist(Types::hex_coord_t a, Types::hex_coord_t b) {
    int dq = a.q - b.q;
    int dr = a.r - b.r;
    return (std::abs(dq) + std::abs(dr) + std::abs(dq + dr)) / 2;
}

} // namespace CityPlanner
