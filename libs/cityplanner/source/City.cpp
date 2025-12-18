#include "CityPlanner/City.h"
namespace CityPlanner {

City::City() {}
City::~City() {}
City::City(config_t config) : m_config(config) {}

const City::config_t &City::GetConfig() const {
    return m_config;
}

} // namespace CityPlanner