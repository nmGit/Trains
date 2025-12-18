#pragma once

#include "interface/worldview/WorldView.h"
#include "ViewModel.h"
#include "engine/Core.h"

namespace Trains {
class Game {
  public:
    Game();

    void Start();

  protected:
  private:
    Trains::Core       m_core;
    Drafter::Canvas    m_canvas;
    Trains::WorldView          m_world_view{m_canvas};
    Trains::ViewModel  m_view_model{m_core, m_world_view};
};
} // namespace Trains
