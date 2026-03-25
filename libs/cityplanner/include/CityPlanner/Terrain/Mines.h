#pragma once
#include "CityPlanner/World.h"
namespace CityPlanner {
class Mines {
  public:
    struct config_t {};
    Mines(config_t &config, World &world);
};
}; // namespace CityPlanner