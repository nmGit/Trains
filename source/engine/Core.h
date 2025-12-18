#pragma once

#include "CityPlanner/World.h"

namespace Trains {
class Core {
  public:
    Core() = default;
  protected:
  private:
    CityPlanner::World m_world;
};
} // namespace Trains