#pragma once
#include "Events/Event.h"
#include "Log/Log.h"
#include "Region.h"
#include "Terrain/Terrain.h"
#include "Types.h"

#include <map>
#include <optional>
#include <random>
#include <vector>

namespace CityPlanner {

/**
 * @brief Represents the state of the world.
 */
class World {
  public:
    typedef Event<void, Region &> region_added_event_t;

    /**
     * @brief Construction parameters for a World.
     */
    struct config_t {
        int width  = 80; ///< World extent in q direction [0, width)
        int height = 80; ///< World extent in r direction [0, height)
    };

    World();
    explicit World(config_t config);

    /** @brief Returns the construction parameters for this world. */
    const config_t &GetConfig() const;

    /** @brief Returns true if the tile is inside the world boundaries. */
    bool InBounds(hex_coord_t tile) const;

    /** @brief Returns tile properties, inserting a default if not yet set. */
    tile_properties_t &GetTile(hex_coord_t tile);

    /** @brief Returns tile properties, or nullptr if not set. */
    const tile_properties_t *GetTileConst(hex_coord_t tile) const;

    /** @brief Returns a const reference to the full tile map. */
    const std::map<hex_coord_t, tile_properties_t> &GetTileMap() const;

    /**
     * @brief Enables per-tick terrain simulation (water flow, forest growth, etc.).
     *
     * Must be called after TerrainGenerator::Generate() so terrain tiles exist.
     *
     * @param config Simulation parameters.
     * @param seed   RNG seed for simulation noise.
     */
    void EnableTerrainService(TerrainGenerator::service_config_t config,
                              uint32_t seed = 42);

    void    Service();
    Region &AddRegion();
    region_added_event_t &OnRegionAdded();

  private:
    void ServiceTerrain();
    config_t                                            m_config;
    std::map<hex_coord_t, tile_properties_t>            m_tiles;
    region_added_event_t                                m_regionAddedEvent;
    std::vector<Region>                                 m_regions;
    std::optional<TerrainGenerator::service_config_t>   m_terrain_config;
    std::mt19937                                        m_terrain_rng;
    LogContext                                          m_log_context{"CityPlanner::World"};
};
} // namespace CityPlanner
