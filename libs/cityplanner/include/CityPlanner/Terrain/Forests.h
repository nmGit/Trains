#pragma once
#include "CityPlanner/World.h"

#include <random>

namespace CityPlanner {

/**
 * @brief Generates and simulates forests using Markov-style directional growth.
 *
 * Forests begin as seed tiles scattered across the world. During each service
 * tick, frontier tiles (forest tiles with at least one non-forest neighbor)
 * have a probability of expanding into a neighbor. The expansion direction is
 * biased by a per-tile growth heading that drifts slowly, producing organic
 * clumps rather than uniform circles.
 *
 * All forest tiles accumulate wood over time at a configurable rate.
 */
class Forests {
  public:
    /**
     * @brief Places initial forest seed tiles in the world.
     *
     * @param config  Seed placement parameters.
     * @param world   The world to populate.
     * @param rng     Seeded random engine.
     */
    static void Generate(const forest_config_t &config, World &world,
                         std::mt19937 &rng);

    /**
     * @brief Advances forest growth and wood production by one tick.
     *
     * @param config  Simulation parameters.
     * @param world   The world whose forest tiles are updated.
     * @param rng     Random engine for growth decisions and heading drift.
     */
    static void Service(const forest_service_config_t &config, World &world,
                        std::mt19937 &rng);
};

} // namespace CityPlanner
