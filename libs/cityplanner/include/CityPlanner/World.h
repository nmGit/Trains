#pragma once
#include "Events/Event.h"
#include "Region.h"
#include "Log/Log.h"

#include <vector>
namespace CityPlanner {
class World {
  public:
    typedef Event<void, Region &> region_added_event_t;

    World();
    void                  AddRegion(Region &r);
    region_added_event_t &OnRegionAdded();

  protected:
  private:
    region_added_event_t m_regionAddedEvent;
    std::vector<Region>  m_regions;
    LogContext           m_log_context{"CityPlanner::World"};
};
} // namespace CityPlanner
