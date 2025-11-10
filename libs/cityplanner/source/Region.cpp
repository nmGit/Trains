#include "CityPlanner/Region.h"
namespace CityPlanner {

Region::Region() {}

Region::city_added_event_t &Region::OnCityAdded() {
    return m_city_added_event;
}

void Region::AddCity(City &c) {
    m_cities.push_back(c);
    m_city_added_event.Emit(c);
    Log::Info(m_log_context, "City added to region. Total cities: %zu",
              m_cities.size());
}
} // namespace CityPlanner
