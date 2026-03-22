#pragma once
#include "CityPlanner/Types.h"
#include <array>
namespace CityPlanner {
point_t                    AxialToPixel(const hex_coord_t &hex);
std::array<hex_coord_t, 6> Neighbors(hex_coord_t hex);
} // namespace CityPlanner