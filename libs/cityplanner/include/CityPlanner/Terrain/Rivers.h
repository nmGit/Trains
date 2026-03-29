#pragma once
#include "CityPlanner/World.h"

#include <random>

namespace CityPlanner {

/**
 * @brief Generates natural-looking rivers that start from world edges and flow
 *        inward.
 *
 * Rivers are produced via a biased random walk with a persistent heading angle.
 * The heading drifts slowly (meander), producing smooth curves rather than
 * jagged paths.  Rivers branch recursively and taper in width from source to
 * terminus.
 *
 * Each river tile stores a downstream pointer so that future simulation can
 * model water flow and consumption.
 */
class Rivers {
  public:
    /**
     * @brief Generates rivers and writes tile properties into the world.
     *
     * @param config  River generation parameters.
     * @param world   The world to populate (reads bounds, writes tiles).
     * @param rng     Seeded random engine.
     */
    static void Generate(const river_config_t &config, World &world,
                         std::mt19937 &rng);

    /**
     * @brief Advances the water simulation by one tick.
     *
     * @param config  Simulation parameters.
     * @param world   The world whose river tiles are updated.
     * @param rng     Random engine for headwater noise.
     */
    static void Service(const river_service_config_t &config, World &world,
                        std::mt19937 &rng);
};

} // namespace CityPlanner
