#pragma once
#include "Types/Types.h"
#include <array>
namespace Types {
point_t                    AxialToPixel(const hex_coord_t &hex);

/**
 * @brief Converts a pixel-space position to the nearest axial hex coordinate.
 *
 * Operates in the same unit space as AxialToPixel (hex_size = 1).
 * Divide world-space coordinates by the cell radius before calling this
 * if your renderer scales hex positions by a cell radius.
 *
 * @param pixel Pixel-space position (same units as AxialToPixel output).
 * @return The nearest hex coordinate in axial form.
 */
hex_coord_t                PixelToAxial(point_t pixel);

std::array<hex_coord_t, 6> Neighbors(hex_coord_t hex);

/**
 * @brief Computes the hex-grid distance between two axial coordinates.
 *
 * Uses the cube-coordinate formula: distance = (|dq| + |dr| + |dq+dr|) / 2.
 *
 * @param a First hex coordinate.
 * @param b Second hex coordinate.
 * @return Number of hex steps between a and b.
 */
int HexDist(hex_coord_t a, hex_coord_t b);
} // namespace CityPlanner