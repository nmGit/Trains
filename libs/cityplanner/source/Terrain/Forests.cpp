#include "CityPlanner/Terrain/Forests.h"
#include "CityPlanner/Utils.h"

#include <cmath>
#include <numbers>
#include <vector>

namespace CityPlanner {

// ---------------------------------------------------------------------------
// Generate — place seed tiles
// ---------------------------------------------------------------------------

static int HexDist(hex_coord_t a, hex_coord_t b) {
    int dq = a.q - b.q;
    int dr = a.r - b.r;
    return (std::abs(dq) + std::abs(dr) + std::abs(dq + dr)) / 2;
}

void Forests::Generate(const forest_config_t &config, World &world,
                       std::mt19937 &rng) {
    const auto &wcfg = world.GetConfig();
    if (wcfg.width <= 0 || wcfg.height <= 0) return;

    std::uniform_int_distribution<int> dist_q(1, wcfg.width - 2);
    std::uniform_int_distribution<int> dist_r(1, wcfg.height - 2);
    std::uniform_real_distribution<float> dist_angle(
        0.f, 2.f * std::numbers::pi_v<float>);

    std::vector<hex_coord_t> seeds;
    seeds.reserve(config.num_seeds);

    int attempts = 0;
    while (static_cast<int>(seeds.size()) < config.num_seeds &&
           attempts < config.num_seeds * 50) {
        ++attempts;
        hex_coord_t candidate{dist_q(rng), dist_r(rng)};

        // Don't place on river tiles or tiles outside the dirt score range.
        const auto *tp = world.GetTileConst(candidate);
        if (tp && tp->is_river) continue;
        if (tp && (tp->dirt_score < config.min_dirt_score ||
                   tp->dirt_score > config.max_dirt_score))
            continue;

        // Enforce minimum spacing between seeds.
        bool too_close = false;
        for (auto &s : seeds) {
            if (HexDist(candidate, s) < config.min_seed_dist) {
                too_close = true;
                break;
            }
        }
        if (too_close) continue;

        seeds.push_back(candidate);

        auto &tile          = world.GetTile(candidate);
        tile.is_forest      = true;
        tile.growth_heading = dist_angle(rng);
    }
}

void Forests::Service(const forest_service_config_t &config, World &world,
                      std::mt19937 &rng) {
    const auto &tiles = world.GetTileMap();

    std::uniform_real_distribution<float> prob(0.f, 1.f);
    std::uniform_real_distribution<float> drift(-config.heading_drift,
                                                 config.heading_drift);

    // Snapshot frontier tiles to avoid mutating while iterating.
    struct frontier_t {
        hex_coord_t coord;
        float       heading;
    };
    std::vector<frontier_t> frontier;

    for (const auto &[coord, props] : tiles) {
        if (!props.is_forest) continue;
        bool is_frontier = false;
        for (auto &nb : Neighbors(coord)) {
            if (!world.InBounds(nb)) continue;
            const auto *nbp = world.GetTileConst(nb);
            if (!nbp || !nbp->is_forest) {
                is_frontier = true;
                break;
            }
        }
        if (is_frontier) {
            frontier.push_back({coord, props.growth_heading});
        }
    }

    // Attempt growth from each frontier tile.
    for (auto &[coord, heading] : frontier) {
        if (prob(rng) > config.growth_probability) continue;

        float new_heading = heading + drift(rng);

        hex_coord_t best{};
        float       best_score = -2.f;

        for (auto &nb : Neighbors(coord)) {
            if (!world.InBounds(nb)) continue;
            const auto *nbp = world.GetTileConst(nb);
            if (nbp && (nbp->is_forest || nbp->is_river)) continue;
            if (nbp && (nbp->dirt_score < config.min_dirt_score ||
                        nbp->dirt_score > config.max_dirt_score))
                continue;

            auto  px    = AxialToPixel(nb);
            auto  pc    = AxialToPixel(coord);
            float dx    = px.x - pc.x;
            float dy    = px.y - pc.y;
            float angle = std::atan2(dy, dx);
            float score = std::cos(angle - new_heading);

            if (score > best_score) {
                best_score = score;
                best       = nb;
            }
        }

        if (best_score > -2.f) {
            auto &tile          = world.GetTile(best);
            tile.is_forest      = true;
            tile.growth_heading = new_heading;
            world.GetTile(coord).growth_heading = new_heading;
        }
    }

    // Wood accumulation on all forest tiles.
    // Max wood is scaled by how well-suited the tile's dirt score is:
    // peaks at 1.0 at the midpoint of [min_dirt, max_dirt], drops to 0 at edges.
    const float dirt_range = config.max_dirt_score - config.min_dirt_score;
    for (const auto &[coord, props] : tiles) {
        if (!props.is_forest) continue;
        auto &tile = world.GetTile(coord);

        float dirt_fit = (dirt_range > 0.f)
            ? std::min(props.dirt_score - config.min_dirt_score,
                       config.max_dirt_score - props.dirt_score) *
                  2.f / dirt_range
            : 1.f;
        float effective_max = config.max_wood * std::clamp(dirt_fit, 0.f, 1.f);

        tile.wood = std::min(tile.wood + config.wood_rate, effective_max);
    }
}

} // namespace CityPlanner
