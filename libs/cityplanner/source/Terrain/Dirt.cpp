#include "CityPlanner/Terrain/Dirt.h"
#include "CityPlanner/Utils.h"

#include <algorithm>
#include <deque>
#include <set>

namespace CityPlanner {

void Dirt::Generate(const dirt_config_t &config, World &world, std::mt19937 &rng) {
    const auto &wcfg = world.GetConfig();
    if (wcfg.width <= 0 || wcfg.height <= 0) return;

    std::uniform_int_distribution<int>    dist_q(0, wcfg.width - 1);
    std::uniform_int_distribution<int>    dist_r(0, wcfg.height - 1);
    std::uniform_real_distribution<float> dist_score(config.min_score,
                                                     config.max_score);
    std::uniform_real_distribution<float> dist_noise(-config.noise, config.noise);

    // --- Phase 1: place seed tiles with random dirt scores ---
    struct bfs_entry_t {
        hex_coord_t coord;
        float       score;
    };

    std::deque<bfs_entry_t> queue;
    std::set<hex_coord_t>   visited;

    int attempts = 0;
    int placed   = 0;
    while (placed < config.num_seeds && attempts < config.num_seeds * 50) {
        ++attempts;
        hex_coord_t c{dist_q(rng), dist_r(rng)};
        if (visited.count(c)) continue;

        float score         = dist_score(rng);
        world.GetTile(c).dirt_score = score;
        visited.insert(c);
        queue.push_back({c, score});
        ++placed;
    }

    // --- Phase 2: multi-source BFS — flood-fill all in-bounds tiles ---
    while (!queue.empty()) {
        auto [coord, score] = queue.front();
        queue.pop_front();

        for (auto &nb : Neighbors(coord)) {
            if (!world.InBounds(nb)) continue;
            if (visited.count(nb)) continue;

            visited.insert(nb);

            float new_score = std::clamp(score + dist_noise(rng), 0.f, 1.f);
            world.GetTile(nb).dirt_score = new_score;
            queue.push_back({nb, new_score});
        }
    }
}

} // namespace CityPlanner
