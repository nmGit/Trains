#include "CityPlanner/City.h"

#include "CityPlanner/World.h"
#include "Types/Utils.h"

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

const std::set<Types::hex_coord_t> &City::GetTiles() const {
    return m_tiles;
}

void City::AbsorbTile(Types::hex_coord_t tile) {
    m_tiles.insert(tile);
}

std::vector<Types::hex_coord_t>
City::GetFrontier(const std::set<Types::hex_coord_t> &region_tiles) const {
    std::set<Types::hex_coord_t>    seen;
    std::vector<Types::hex_coord_t> frontier;
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

float City::ComputeThreshold(Types::hex_coord_t tile) const {
    int count = 0;
    for (const auto &neighbor : Neighbors(tile)) {
        if (m_tiles.count(neighbor))
            ++count;
    }
    const float area = static_cast<float>(m_tiles.size());
    float       p    = m_config.growth.p_base +
              m_config.growth.bias * static_cast<float>(count) +
              m_config.growth.area_bias * area;
    return std::clamp(p, 0.0f, 1.0f);
}

const raw_resources_t &City::GetRawResources() const {
    return m_raw_resources;
}

void City::Service(const World &world) {
    m_raw_resources = {};

    std::set<Types::hex_coord_t> counted_river_tiles;

    for (const auto &tile : m_tiles) {
        const auto *props = world.GetTileConst(tile);
        if (props) {
            m_raw_resources.wood += props->wood;
            m_raw_resources.dirt += props->dirt_score;
        }

        // Accumulate water from bordering river tiles.
        for (const auto &neighbor : Neighbors(tile)) {
            if (m_tiles.count(neighbor) > 0)
                continue; // owned tile, not a border river
            const auto *nprops = world.GetTileConst(neighbor);
            if (!nprops || !nprops->is_river)
                continue;
            if (counted_river_tiles.insert(neighbor).second)
                m_raw_resources.water += nprops->water;
        }
    }
}

} // namespace CityPlanner
