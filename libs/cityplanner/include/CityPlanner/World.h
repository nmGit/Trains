#pragma once
#include "Events/Event.h"
#include "Log/Log.h"
#include "Region.h"

#include <vector>
namespace CityPlanner {
class World {
  public:
    typedef Event<void, Region &> region_added_event_t;

    World();
    Region& AddRegion();

    region_added_event_t &OnRegionAdded();

  protected:
  private:
    region_added_event_t  m_regionAddedEvent;
    std::vector<Region > m_regions;
    LogContext            m_log_context{"CityPlanner::World"};
};
} // namespace CityPlanner
