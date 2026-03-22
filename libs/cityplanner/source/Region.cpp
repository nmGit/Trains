#include "CityPlanner/Region.h"
#include "CityPlanner/Utils.h"

#include <limits>
#include <map>
#include <random>
#include <set>

namespace CityPlanner {

namespace {

std::mt19937 &Rng() {
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}

float RandFloat() {
    return std::uniform_real_distribution<float>{0.0f, 1.0f}(Rng());
}

} // namespace

Region::Region() {}

Region::city_added_event_t &Region::OnCityAdded() {
    return m_city_added_event;
}

void Region::Service() {
    GrowCities();
    for (auto &c : m_cities) {
        c.Service();
    }
}

void Region::GrowCities() {
    if (m_cities.empty()) return;

    // Build the union of all tiles claimed by any city so that frontier
    // candidates are never tiles already owned by another city.
    std::set<hex_coord_t> all_claimed;
    for (const auto &city : m_cities) {
        all_claimed.insert(city.GetTiles().begin(), city.GetTiles().end());
    }

    // perimeter tracks the winning bid for each candidate tile.
    // A city wins a tile only if its roll exceeds its own personalised
    // threshold AND the roll of every other competing city.
    struct bid_t {
        size_t city_index = std::numeric_limits<size_t>::max(); // sentinel: no winner
        float  roll       = 0.0f;
    };
    std::map<hex_coord_t, bid_t> perimeter;

    for (size_t i = 0; i < m_cities.size(); ++i) {
        const auto &city = m_cities[i];

        // Frontier: neighbors of this city's tiles not claimed by any city.
        std::set<hex_coord_t>    seen;
        std::vector<hex_coord_t> frontier;
        for (const auto &tile : city.GetTiles()) {
            for (const auto &neighbor : Neighbors(tile)) {
                if (all_claimed.count(neighbor) == 0 && seen.insert(neighbor).second) {
                    frontier.push_back(neighbor);
                }
            }
        }

        // Each frontier tile is contested: a city bids with a random roll that
        // must exceed its personalised threshold. The highest valid roll wins.
        // Only unclaimed tiles are eligible to enter the perimeter.
        for (const auto &tile : frontier) {
            if (all_claimed.count(tile) > 0) continue;
            float threshold = city.ComputeThreshold(tile);
            float roll      = RandFloat();
            if (roll > threshold) {
                auto &bid = perimeter[tile];
                if (roll > bid.roll) {
                    bid = {i, roll};
                }
            }
        }
    }

    // Commit: award each contested tile to the city with the winning bid.
    // Guard against absorbing a tile that is already owned by another city.
    for (const auto &[tile, bid] : perimeter) {
        if (bid.city_index != std::numeric_limits<size_t>::max() &&
            all_claimed.count(tile) == 0) {
            m_cities[bid.city_index].AbsorbTile(tile);
        }
    }
}

} // namespace CityPlanner
