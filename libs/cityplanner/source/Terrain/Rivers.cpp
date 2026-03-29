#include "CityPlanner/Terrain/Rivers.h"

#include "CityPlanner/Utils.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <set>
#include <vector>

namespace CityPlanner {

namespace {

/// A single tile on the river center-line.
struct river_node_t {
    hex_coord_t coord;
    hex_coord_t downstream; ///< Next tile in flow direction (self = terminal)
    int         width;      ///< Width to expand at this tile
};

/// Returns the angle (radians) from hex `a` to hex `b` in pixel space.
float AngleBetween(hex_coord_t a, hex_coord_t b) {
    auto pa = AxialToPixel(a);
    auto pb = AxialToPixel(b);
    return std::atan2(pb.y - pa.y, pb.x - pa.x);
}

/// Collects all tiles on the world boundary.
std::vector<hex_coord_t> CollectEdgeTiles(const World &world) {
    const auto              &cfg = world.GetConfig();
    std::vector<hex_coord_t> edges;
    for (int q = 0; q < cfg.width; ++q) {
        edges.push_back({q, 0});
        edges.push_back({q, cfg.height - 1});
    }
    for (int r = 1; r < cfg.height - 1; ++r) {
        edges.push_back({0, r});
        edges.push_back({cfg.width - 1, r});
    }
    return edges;
}

/// Returns the center of the world in axial coords.
hex_coord_t WorldCenter(const World &world) {
    const auto &cfg = world.GetConfig();
    return {cfg.width / 2, cfg.height / 2};
}

/// Walks a single river center-line and appends nodes to `out`.
/// `center_tiles` tracks all center-line tiles across all rivers/branches
/// so that paths don't cross each other.
void WalkRiver(const river_config_t &config, const World &world,
               std::mt19937 &rng, std::set<hex_coord_t> &center_tiles,
               std::vector<river_node_t> &out, hex_coord_t start, float heading,
               int max_length, int base_width, int depth) {
    constexpr float k_pi = std::numbers::pi_v<float>;

    std::uniform_real_distribution<float> unit_dist(0.f, 1.f);
    std::uniform_real_distribution<float> sign_dist(-1.f, 1.f);

    hex_coord_t current            = start;
    int         steps_since_branch = 0;

    // Index of the first node this walk appends (for downstream linking).
    size_t first_idx = out.size();

    // Record the start tile.
    center_tiles.insert(current);
    out.push_back({current, current, base_width}); // downstream set later

    for (int step = 0; step < max_length; ++step) {
        // --- Meander: randomly perturb heading ---
        if (unit_dist(rng) < config.meander) {
            heading += (sign_dist(rng) > 0.f ? 1.f : -1.f) * (k_pi / 6.f);
        }

        // --- Choose best neighbor aligned with heading ---
        auto        neighbors = Neighbors(current);
        hex_coord_t best{};
        float       best_score = -2.f;
        bool        found      = false;

        for (auto &nb : neighbors) {
            if (!world.InBounds(nb))
                continue;
            if (center_tiles.count(nb))
                continue; // avoid existing center-lines

            float angle = AngleBetween(current, nb);
            float score = std::cos(angle - heading);
            if (score > best_score) {
                best_score = score;
                best       = nb;
                found      = true;
            }
        }

        if (!found)
            break; // stuck

        // --- Width: taper + noise + forced tail taper ---
        float raw_w = static_cast<float>(base_width) -
                      static_cast<float>(step) * config.width_taper;

        // Random perturbation.
        if (config.width_noise > 0.f) {
            raw_w += sign_dist(rng) * config.width_noise *
                     static_cast<float>(base_width);
        }

        // Force taper to min_width over the last `taper_tail` tiles.
        int remaining = max_length - step;
        if (config.taper_tail > 0 && remaining < config.taper_tail) {
            float t = static_cast<float>(remaining) /
                      static_cast<float>(config.taper_tail);
            raw_w = static_cast<float>(config.min_width) +
                    t * (raw_w - static_cast<float>(config.min_width));
        }

        int width =
            std::max(config.min_width, static_cast<int>(std::round(raw_w)));

        // --- Record node ---
        center_tiles.insert(best);
        out.push_back(
            {best, best, width}); // downstream = self (terminal) for now

        // Link previous node's downstream to this one.
        out[out.size() - 2].downstream = best;

        current = best;
        ++steps_since_branch;

        // --- Branching ---
        if (depth < config.branch_max_depth &&
            steps_since_branch >= config.min_branch_spacing &&
            step >= config.min_length / 2 &&
            unit_dist(rng) < config.branch_probability) {
            float branch_heading =
                heading + (sign_dist(rng) > 0.f ? 1.f : -1.f) * (k_pi / 3.f);
            int branch_length =
                static_cast<int>(static_cast<float>(max_length - step) * 0.6f);
            int branch_width = std::max(config.min_width, width - 1);

            if (branch_length > config.min_length / 2) {
                WalkRiver(config, world, rng, center_tiles, out, current,
                          branch_heading, branch_length, branch_width,
                          depth + 1);
            }
            steps_since_branch = 0;
        }
    }
}

/// Expands width around a center-line tile using BFS, writing to the world.
void ExpandWidth(World &world, const std::set<hex_coord_t> &center_tiles,
                 hex_coord_t center, int radius, hex_coord_t downstream,
                 int width_val) {
    std::vector<hex_coord_t> frontier = {center};
    std::set<hex_coord_t>    visited  = {center};

    for (int dist = 0; dist < radius; ++dist) {
        std::vector<hex_coord_t> next_frontier;
        for (auto &tile : frontier) {
            for (auto &nb : Neighbors(tile)) {
                if (!world.InBounds(nb))
                    continue;
                if (visited.count(nb))
                    continue;
                if (center_tiles.count(nb))
                    continue; // don't overwrite center-line
                visited.insert(nb);
                next_frontier.push_back(nb);

                auto &props       = world.GetTile(nb);
                props.is_river    = true;
                props.downstream  = downstream;
                props.river_width = width_val;
            }
        }
        frontier = std::move(next_frontier);
    }
}

} // anonymous namespace

void Rivers::Generate(const river_config_t &config, World &world,
                      std::mt19937 &rng) {
    auto edges  = CollectEdgeTiles(world);
    auto center = WorldCenter(world);

    std::shuffle(edges.begin(), edges.end(), rng);

    // Phase 1: Walk all center-lines (no world writes yet).
    std::set<hex_coord_t>     center_tiles;
    std::vector<river_node_t> nodes;

    int count = std::min(config.num_rivers, static_cast<int>(edges.size()));
    for (int i = 0; i < count; ++i) {
        hex_coord_t source  = edges[i];
        float       heading = AngleBetween(source, center);

        WalkRiver(config, world, rng, center_tiles, nodes, source, heading,
                  config.max_length, config.base_width, 0);
    }

    // Phase 2: Write center-line tiles to the world.
    for (auto &node : nodes) {
        auto &props       = world.GetTile(node.coord);
        props.is_river    = true;
        props.downstream  = node.downstream;
        props.river_width = node.width;
    }

    // Phase 3: Expand width around each center-line tile.
    for (auto &node : nodes) {
        if (node.width > 1) {
            ExpandWidth(world, center_tiles, node.coord, node.width / 2,
                        node.downstream, node.width);
        }
    }
}

void Rivers::Service(const river_service_config_t &config, World &world,
                     std::mt19937 &rng) {
    const auto &wcfg  = world.GetConfig();
    const auto &tiles = world.GetTileMap();

    std::uniform_real_distribution<float> noise_dist(-config.headwater_noise,
                                                     config.headwater_noise);

    // Accumulate deltas separately to avoid iteration-order dependence.
    std::map<hex_coord_t, float> deltas;

    for (const auto &[coord, props] : tiles) {
        if (!props.is_river)
            continue;

        // --- Headwater production ---
        bool is_headwater = (coord.q == 0 || coord.q == wcfg.width - 1 ||
                             coord.r == 0 || coord.r == wcfg.height - 1);
        if (is_headwater) {
            float inflow =
                std::max(0.f, config.headwater_rate + noise_dist(rng));
            deltas[coord] += inflow;
        }

        // --- Diffusion: water flows from high to low neighbors ---
        if (props.water < config.transfer_threshold)
            continue;

        auto neighbors           = Neighbors(coord);
        int  num_river_neighbors = 0;
        for (auto &nb : neighbors) {
            const auto *nbp = world.GetTileConst(nb);
            if (nbp && nbp->is_river)
                ++num_river_neighbors;
        }
        if (num_river_neighbors == 0)
            continue;

        float per_neighbor_rate =
            config.transfer_rate / static_cast<float>(num_river_neighbors);

        for (auto &nb : neighbors) {
            const auto *nbp = world.GetTileConst(nb);
            if (!nbp || !nbp->is_river)
                continue;

            if (props.water > nbp->water) {
                float transfer =
                    (props.water - nbp->water) * per_neighbor_rate +
                    config.transfer_constant;
                deltas[coord] -= transfer;
                deltas[nb] += transfer;
            }
        }
    }

    // Apply deltas, then redistribute any overflow to river neighbors.
    for (auto &[coord, delta] : deltas) {
        auto &props = world.GetTile(coord);
        props.water = std::max(props.water + delta, 0.f);
    }

    // Overflow pass: push excess water beyond max into river neighbors.
    // Use a queue so overflow cascades through cells not in deltas.
    std::vector<hex_coord_t> overflow_queue;
    for (auto &[coord, _] : deltas) {
        if (world.GetTile(coord).water > config.max_water)
            overflow_queue.push_back(coord);
    }

    constexpr int k_max_overflow_iters = 1000;
    for (int i = 0; i < static_cast<int>(overflow_queue.size()) &&
                    i < k_max_overflow_iters;
         ++i) {
        auto &props = world.GetTile(overflow_queue[i]);
        if (props.water <= config.max_water)
            continue;

        float excess = props.water - config.max_water;
        props.water  = config.max_water;

        auto neighbors           = Neighbors(overflow_queue[i]);
        int  num_river_neighbors = 0;
        for (auto &nb : neighbors) {
            const auto *nbp = world.GetTileConst(nb);
            if (nbp && nbp->is_river)
                ++num_river_neighbors;
        }
        if (num_river_neighbors == 0)
            continue;

        float per_neighbor = excess / static_cast<float>(num_river_neighbors);
        for (auto &nb : neighbors) {
            const auto *nbp = world.GetTileConst(nb);
            if (!nbp || !nbp->is_river)
                continue;
            auto &nb_props = world.GetTile(nb);
            nb_props.water += per_neighbor;
            if (nb_props.water > config.max_water)
                overflow_queue.push_back(nb);
        }
    }
}

} // namespace CityPlanner
