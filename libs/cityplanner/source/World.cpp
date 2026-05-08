#include "CityPlanner/World.h"
#include "CityPlanner/Terrain/Terrain.h"

namespace CityPlanner {

World::World() {}

World::World(config_t config) : m_config(config) {}

const World::config_t &World::GetConfig() const {
    return m_config;
}

bool World::InBounds(Types::hex_coord_t tile) const {
    if (tile.q < 0 || tile.q >= m_config.width) return false;
    int offset_r = tile.r + tile.q / 2;
    return offset_r >= 0 && offset_r < m_config.height;
}

World::tile_properties_t &World::GetTile(Types::hex_coord_t tile) {
    return m_tiles[tile];
}

const World::tile_properties_t *World::GetTileConst(Types::hex_coord_t tile) const {
    auto it = m_tiles.find(tile);
    return it != m_tiles.end() ? &it->second : nullptr;
}

const std::map<Types::hex_coord_t, World::tile_properties_t> &World::GetTileMap() const {
    return m_tiles;
}

void World::EnableTerrainService(TerrainGenerator::service_config_t config,
                                 uint32_t seed) {
    m_terrain_config = config;
    m_terrain_rng.seed(seed);
}

Region &World::AddRegion() {
    auto &r = m_regions.emplace_back();
    m_regionAddedEvent.Emit(r);
    Log::Info(m_log_context, "Region added to world. Total regions: %zu",
              m_regions.size());
    return r;
}

World::region_added_event_t &World::OnRegionAdded() {
    return m_regionAddedEvent;
}

void World::Service() {
    for (auto &r : m_regions) {
        r.Service(*this);
    }
    if (m_terrain_config) {
        ServiceTerrain();
    }
}

void World::ServiceTerrain() {
    TerrainGenerator::Service(*m_terrain_config, *this, m_terrain_rng);
}

} // namespace CityPlanner
