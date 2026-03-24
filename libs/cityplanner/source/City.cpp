#include "CityPlanner/City.h"
#include "CityPlanner/Utils.h"

#include <algorithm>

namespace CityPlanner {

City::City() {}
City::~City() {}
City::City(config_t config) : m_config(config) {
    m_tiles.insert(config.location);
}

const City::config_t &City::GetConfig() const {
    return m_config;
}

const std::set<hex_coord_t> &City::GetTiles() const {
    return m_tiles;
}

void City::AbsorbTile(hex_coord_t tile) {
    m_tiles.insert(tile);
}

std::vector<hex_coord_t> City::GetFrontier(const std::set<hex_coord_t> &region_tiles) const {
    std::set<hex_coord_t>    seen;
    std::vector<hex_coord_t> frontier;
    for (const auto &tile : m_tiles) {
        for (const auto &neighbor : Neighbors(tile)) {
            if (m_tiles.count(neighbor) == 0 &&
                region_tiles.count(neighbor) > 0 &&
                seen.insert(neighbor).second) {
                frontier.push_back(neighbor);
            }
        }
    }
    return frontier;
}

float City::ComputeThreshold(hex_coord_t tile) const {
    int count = 0;
    for (const auto &neighbor : Neighbors(tile)) {
        if (m_tiles.count(neighbor)) ++count;
    }
    const float area = static_cast<float>(m_tiles.size());
    float p = m_config.growth.p_base
            + m_config.growth.bias      * static_cast<float>(count)
            + m_config.growth.area_bias * area;
    return std::clamp(p, 0.0f, 1.0f);
}

void City::Service() {}

} // namespace CityPlanner
