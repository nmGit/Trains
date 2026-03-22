#pragma once

#include "CityPlanner/City.h"
#include "Events/Event.h"
#include "Log/Log.h"

#include <deque>

namespace CityPlanner {

/**
 * @brief Represents a geographical region containing cities.
 *
 * Worlds are made of multiple regions. Each Service() tick the region runs
 * the city-growth competition, allowing cities to expand within the region.
 * Region boundary growth is not yet implemented.
 */
class Region {
  public:
    /**
     * @brief Event type emitted when a city is added to this region.
     */
    typedef Event<void, City &> city_added_event_t;

    /**
     * @brief Constructs a new Region object.
     */
    Region();

    /**
     * @brief Adds a city to the region and emits the city_added event.
     * @param args Arguments forwarded to the City constructor.
     * @return A reference to the newly created City.
     */
    template <typename... Args>
    City &AddCity(Args &&...args) {
        auto &c = m_cities.emplace_back(std::forward<Args>(args)...);
        m_city_added_event.Emit(c);
        Log::Info(m_log_context, "City added to region. Total cities: %zu",
                  m_cities.size());
        return c;
    }

    /**
     * @brief Returns the event fired whenever a city is added to this region.
     * @return A reference to the city_added_event_t event object.
     */
    city_added_event_t &OnCityAdded();

    /**
     * @brief Advances the region by one simulation tick.
     *
     * Runs the city-growth competition and then services each city.
     */
    void Service();

  private:
    /**
     * @brief Runs one growth tick for all cities within this region.
     *
     * For each unclaimed tile on any city's frontier, cities compete using
     * their individual growth parameters. The city with the highest roll
     * that also exceeds its own threshold claims the tile.
     */
    void GrowCities();

    city_added_event_t m_city_added_event;
    std::deque<City>   m_cities; // deque: emplace_back never invalidates existing pointers
    LogContext         m_log_context{"CityPlanner::Region"};
};

} // namespace CityPlanner
