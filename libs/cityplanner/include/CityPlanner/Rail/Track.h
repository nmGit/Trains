#pragma once

#include <array>
#include <cstddef>

namespace CityPlanner::Track {

/**
 * @brief Identifies a track type.  The integer value of each enumerator is
 *        used as an index into k_configs, so the order here must match the
 *        initialiser list of that array.  @p kCount must remain the last entry.
 */
enum class track_type_t {
    kBasicTrack,
    kCount ///< Sentinel — keep last; used to size k_configs at compile time.
};

/** @brief Parameters that govern the behaviour of a track type. */
struct config_t {
    float speed_multiplier    = 1.0f;  ///< Relative to kBasicTrack.
    float capacity_multiplier = 1.0f;  ///< Relative to kBasicTrack.
    bool  can_cross_rivers    = false;  ///< Whether this track may be routed over river tiles.
};

/**
 * @brief Compile-time table of track configurations, one entry per track_type_t.
 *
 * Adding a new enumerator before kCount without adding a matching entry here
 * produces a compile error, keeping the enum and the table in sync.
 */
inline constexpr std::array<config_t,
                             static_cast<std::size_t>(track_type_t::kCount)> k_configs = {{
    { .speed_multiplier = 1.0f, .capacity_multiplier = 1.0f, .can_cross_rivers = false }, // kBasicTrack
}};

/**
 * @brief Returns the config for the given track type.
 *
 * @param type A valid track_type_t value (not kCount).
 * @return Reference into k_configs; lifetime is that of the static array.
 */
inline const config_t& GetConfig(track_type_t type) noexcept {
    return k_configs[static_cast<std::size_t>(type)];
}

} // namespace CityPlanner::Track
