#pragma once

#include "CityPlanner/Types.h"

namespace CityPlanner {
/**
 * @brief Represents a city
 */
class City {
  public:
    struct config_t {
        const char *name;
        point_t     location;
    };
    City();
    ~City();
    City(config_t config);
    const config_t &GetConfig() const;

  protected:
  private:
    config_t m_config;
};
} // namespace CityPlanner