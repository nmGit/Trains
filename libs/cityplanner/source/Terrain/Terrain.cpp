#include "CityPlanner/Terrain/Terrain.h"
#include "CityPlanner/Terrain/Dirt.h"
#include "CityPlanner/Terrain/Forests.h"
#include "CityPlanner/Terrain/Rivers.h"

#include <random>

namespace CityPlanner {

void TerrainGenerator::Generate(const config_t &config, World &world) {
    std::mt19937 rng(config.seed);

    Dirt::Generate(config.dirt, world, rng);
    Rivers::Generate(config.rivers, world, rng);
    Forests::Generate(config.forests, world, rng);
}

void TerrainGenerator::Service(const service_config_t &config, World &world,
                               std::mt19937 &rng) {
    Rivers::Service(config.rivers, world, rng);
    Forests::Service(config.forests, world, rng);
}

} // namespace CityPlanner
