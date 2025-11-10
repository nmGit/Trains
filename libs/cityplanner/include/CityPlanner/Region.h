#pragma once

#include "CityPlanner/City.h"
#include "Events/Event.h"
#include "Log/Log.h"

namespace CityPlanner {

/**
 * @brief Represents a geographical region
 *
 * Worlds are made of multiple regions, which contain cities and other
 * geographical features.
 */
class Region {
  public:
    /**
     * @brief Defines a type alias for an event that is triggered when a city is
     * added.
     */
    typedef Event<void, City &> city_added_event_t;

    /**
     * @brief Constructs a new Region object.
     */
    Region();

    /**
     * @brief Adds a city to the region.
     * @param c Reference to the City object to add.
     */
    void AddCity(City &c);

    /**
     * @brief Returns a reference to the event that is triggered when a city is
     * added.
     * @return A reference to the city_added_event_t event object.
     */
    city_added_event_t &OnCityAdded();

  protected:
  private:
    city_added_event_t m_city_added_event;
    std::vector<City>  m_cities;
    LogContext         m_log_context{"CityPlanner::Region"};
};
} // namespace CityPlanner
