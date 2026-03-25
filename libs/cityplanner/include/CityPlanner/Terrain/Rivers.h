#pragma once
#include "CityPlanner/World.h"
namespace CityPlanner {
class Rivers {
  public:
    struct config_t {};
    Rivers(config_t &config, World &world);
};
}; // namespace CityPlanner