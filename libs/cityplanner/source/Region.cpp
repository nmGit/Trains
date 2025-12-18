#include "CityPlanner/Region.h"
namespace CityPlanner {

Region::Region() {}

Region::city_added_event_t &Region::OnCityAdded() {
    return m_city_added_event;
}

} // namespace CityPlanner
