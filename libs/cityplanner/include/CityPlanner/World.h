#pragma once
#include "Events/Event.h"
#include "Log/Log.h"
#include "Region.h"
#include "Terrain/Terrain.h"
#include "Types/Types.h"

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
     * @brief Per-tile terrain properties.
     *
     * Stores terrain features (rivers, forests, etc.) for a single hex tile.
     * Tiles with no special properties use the default-constructed state.
     */
    struct tile_properties_t {
        bool               is_river = false;
        Types::hex_coord_t downstream{}; ///< Next tile this river feeds into
                                         ///< (valid when is_river)
        int   river_width = 0; ///< Visual width at this point (1 = center only)
        float water = 0.f; ///< Current water level (accumulated by simulation)

        bool  is_forest = false;
        float wood      = 0.f; ///< Accumulated wood resource
        float growth_heading =
            0.f; ///< Current growth direction (radians), drifts over time

        float dirt_score =
            0.f; ///< Soil quality [0, 1] — determines what may grow here
    };

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
    bool InBounds(Types::hex_coord_t tile) const;

    /** @brief Returns tile properties, inserting a default if not yet set. */
    tile_properties_t &GetTile(Types::hex_coord_t tile);

    /** @brief Returns tile properties, or nullptr if not set. */
    const tile_properties_t *GetTileConst(Types::hex_coord_t tile) const;

    /** @brief Returns a const reference to the full tile map. */
    const std::map<Types::hex_coord_t, tile_properties_t> &GetTileMap() const;

    /**
     * @brief Enables per-tick terrain simulation (water flow, forest growth,
     * etc.).
     *
     * Must be called after TerrainGenerator::Generate() so terrain tiles exist.
     *
     * @param config Simulation parameters.
     * @param seed   RNG seed for simulation noise.
     */
    void EnableTerrainService(TerrainGenerator::service_config_t config,
                              uint32_t                           seed = 42);

    void                  Service();
    Region               &AddRegion();
    region_added_event_t &OnRegionAdded();

  private:
    void                                              ServiceTerrain();
    config_t                                          m_config;
    std::map<Types::hex_coord_t, tile_properties_t>   m_tiles;
    region_added_event_t                              m_regionAddedEvent;
    std::vector<Region>                               m_regions;
    std::optional<TerrainGenerator::service_config_t> m_terrain_config;
    std::mt19937                                      m_terrain_rng;
    LogContext m_log_context{"CityPlanner::World"};
};
} // namespace CityPlanner
