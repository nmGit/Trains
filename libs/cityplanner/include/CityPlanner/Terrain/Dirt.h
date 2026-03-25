#pragma once

#include "CityPlanner/World.h"

namespace CityPlanner {
class Dirt {
  public:
    struct config_t {};
    Dirt(config_t &config, World &world);
};
}; // namespace CityPlanner