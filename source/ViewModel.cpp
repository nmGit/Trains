#include "ViewModel.h"

namespace Trains {

ViewModel::ViewModel(Trains::Core &core, Trains::WorldView &worldview)
    : m_core(core), m_world_view(worldview) {}

}; // namespace Trains