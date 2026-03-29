#pragma once
#include <cstdint>
#include <random>

namespace CityPlanner {

// Forward declaration — World is defined in World.h, which includes this
// header.
class World;

// ---------------------------------------------------------------------------
// Per-feature config structs (generation + service)
// ---------------------------------------------------------------------------

/** @brief Parameters for dirt score generation. */
struct dirt_config_t {
    int   num_seeds = 20; ///< Number of seed tiles with random dirt scores
    float noise =
        0.03f; ///< Score drift per BFS hop (controls gradient smoothness)
    float min_score = 0.1f; ///< Minimum dirt score assigned to seeds
    float max_score = 1.0f; ///< Maximum dirt score assigned to seeds
};

/** @brief Parameters for river generation. */
struct river_config_t {
    int   num_rivers = 4;    ///< Source rivers spawned at world edges
    int   min_length = 20;   ///< Minimum tiles before river can stop
    int   max_length = 100;  ///< Maximum tiles before forced stop
    float meander    = 0.3f; ///< Per-step probability of heading deviation
    float branch_probability =
        0.03f;                    ///< Per-step probability of spawning a branch
    int   min_branch_spacing = 8; ///< Minimum tiles between branch points
    int   branch_max_depth   = 2; ///< Maximum recursive branching depth
    int   base_width         = 2; ///< Width at the source (edge)
    int   min_width          = 1; ///< Minimum width at terminus
    float width_taper        = 0.02f; ///< Width reduction per step
    float width_noise =
        0.4f; ///< Random width perturbation amplitude (0 = none)
    int taper_tail =
        10; ///< Tiles before end where width forces down to min_width
};

/** @brief Runtime simulation parameters for river water flow. */
struct river_service_config_t {
    float transfer_rate =
        0.1f; ///< Fraction of water difference transferred per tick
    float transfer_constant =
        0.1f; ///< Fraction of water difference transferred per tick
    float transfer_threshold =
        5.0f; ///< Minimum water level before a tile diffuses outward
    float headwater_rate =
        0.1f; ///< Base water produced per tick at source tiles
    float headwater_noise =
        0.2f;                ///< Random noise amplitude on headwater production
    float max_water = 50.f; ///< Water level cap per tile
};

/** @brief Parameters for initial forest seed placement. */
struct forest_config_t {
    int   num_seeds     = 12;  ///< Number of initial forest seed tiles
    int   min_seed_dist = 8;   ///< Minimum hex distance between seeds
    float min_dirt_score = 0.5f; ///< Minimum dirt score required for seed placement
    float max_dirt_score = 0.75f; ///< Maximum dirt score that allows seed placement
};

/** @brief Runtime simulation parameters for forest growth and wood production.
 */
struct forest_service_config_t {
    float growth_probability =
        0.02f; ///< Per-frontier-tile chance of expansion per tick
    float heading_drift  = 0.3f;  ///< Max heading change (radians) per expansion
    float wood_rate      = 0.05f; ///< Wood accumulated per forest tile per tick
    float max_wood       = 10.f;  ///< Wood level cap per tile
    float min_dirt_score = 0.5f;   ///< Minimum dirt score required for forest growth
    float max_dirt_score = 0.75f;   ///< Maximum dirt score that allows forest growth
};

// ---------------------------------------------------------------------------
// TerrainGenerator — orchestrates generation and per-tick simulation
// ---------------------------------------------------------------------------

/**
 * @brief Generates terrain features for the world.
 *
 * The world's base map is a hexagonal grid of tiles. Different areas of the
 * world may provide different raw materials, be uninhabitable mountains or
 * deserts, or be covered by water.
 *
 * The terrain generator is responsible for assigning these properties to
 * map tiles, usually when the world is created. Generation is driven by a
 * configurable random seed for reproducibility.
 */
class TerrainGenerator {
  public:
    struct config_t {
        uint32_t        seed = 42;
        dirt_config_t   dirt;
        river_config_t  rivers;
        forest_config_t forests;
    };

    /**
     * @brief Runtime simulation parameters for all terrain features.
     */
    struct service_config_t {
        river_service_config_t  rivers;
        forest_service_config_t forests;
    };

    /**
     * @brief Generates all terrain features for the given world.
     *
     * Constructs an RNG from the seed and runs each sub-generator in sequence.
     *
     * @param config Generation parameters (seed + per-feature configs).
     * @param world  The world to populate with terrain data.
     */
    static void Generate(const config_t &config, World &world);

    /**
     * @brief Advances all terrain simulations by one tick.
     *
     * @param config  Simulation parameters for each terrain feature.
     * @param world   The world whose terrain tiles are updated.
     * @param rng     Random engine (shared across features).
     */
    static void Service(const service_config_t &config, World &world,
                        std::mt19937 &rng);
};

} // namespace CityPlanner
