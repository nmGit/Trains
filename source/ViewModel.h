#pragma once
#include "engine/Core.h"
#include "interface/worldview/WorldView.h"

namespace Trains {

/**
 * @brief The ViewModel class serves as an intermediary between the Core engine
 *        and the WorldView, facilitating communication and data exchange.
 *
 * It does not own either, but it acts as the information pipeline. The
 * viewmodel (or related classes) can also be created to mock or extended for
 * testing or additional functionality.
 */
class ViewModel {
  public:
    ViewModel(Trains::Core &core, Trains::WorldView &worldview);

  protected:
  private:
    Trains::Core      &m_core;
    Trains::WorldView &m_world_view;
};
} // namespace Trains