#include "CityPlanner/World.h"

namespace CityPlanner {

World::World() {
}

void World::AddRegion(Region &r) {
    m_regions.push_back(r);
    m_regionAddedEvent.Emit(r);
    Log::Info(m_log_context, "Region added to world. Total regions: %zu",
              m_regions.size());
}

World::region_added_event_t &World::OnRegionAdded() {
    return m_regionAddedEvent;
}

} // namespace CityPlanner