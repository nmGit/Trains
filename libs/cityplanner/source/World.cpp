#include "CityPlanner/World.h"

namespace CityPlanner {

World::World() {}

Region &World::AddRegion() {
    auto &r = m_regions.emplace_back();
    m_regionAddedEvent.Emit(r);
    Log::Info(m_log_context, "Region added to world. Total regions: %zu",
              m_regions.size());
    return r;
}

World::region_added_event_t &World::OnRegionAdded() {
    return m_regionAddedEvent;
}

} // namespace CityPlanner
