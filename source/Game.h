#pragma once

#include "interface/worldview/WorldView.h"

namespace Trains {
class Game {
  public:
    Game();

    void Start();

  protected:
  private:
    CityPlanner::World m_world;
    Drafter::Canvas    m_canvas;
    WorldView          m_world_view{m_world, m_canvas};
};
} // namespace Trains
