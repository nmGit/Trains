#pragma once
#include "CityPlanner/World.h"

#include <random>

namespace CityPlanner {

/**
 * @brief Assigns a static dirt score to every tile in the world.
 *
 * Dirt scores determine what may grow on a tile (e.g. forests, crops).
 * Generation uses a multi-source BFS from randomly placed seed tiles,
 * producing a Voronoi-like pattern where each region has a base score
 * that drifts gradually with distance from the seed.
 *
 * Dirt scores are assigned once at world creation and do not change.
 */
class Dirt {
  public:
    /**
     * @brief Assigns dirt scores to all in-bounds tiles via multi-source BFS.
     *
     * @param config  Generation parameters.
     * @param world   The world to populate with dirt scores.
     * @param rng     Seeded random engine.
     */
    static void Generate(const dirt_config_t &config, World &world,
                         std::mt19937 &rng);
};

} // namespace CityPlanner
