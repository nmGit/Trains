#pragma once
#include "CityPlanner/World.h"
#include "CityPlanner/Terrain/Rivers.h"
#include "CityPlanner/Terrain/Mines.h"
#include "CityPlanner/Terrain/Forests.h"
#include "CityPlanner/Terrain/Dirt.h"

namespace CityPlanner {
/**
 * @brief Generates terrain features for the world.
 *
 * The world's base map is a hexagonal grid of tiles. Different areas of the
 * world may provide different raw materials, be uninhabitable moutains or
 * deserts, or be covered by water.
 *
 * The terrain generator is responsible for assigning these properties to
 * map tiles, usually when the world is created.
 */
class TerrainGenerator {
  public:
      struct config_t {
        Rivers::config_t rivers_config;
    };
    /**
     * @brief Constructs a TerrainGenerator and populates the world's terrain.
     *
     * @param world The world to generate terrain for.
     */
    TerrainGenerator(World &world);
};
} // namespace CityPlanner