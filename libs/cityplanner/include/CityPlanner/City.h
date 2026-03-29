#pragma once

#include "CityPlanner/Types.h"

#include <set>
#include <vector>

namespace CityPlanner {

/**
 * @brief Represents a city within a region.
 *
 * A city begins as a single seed tile and expands outward over time via the
 * growth algorithm driven by its parent Region. The set of claimed tiles is
 * updated each simulation tick by Region::GrowCities().
 */
class City {
  public:
    /**
     * @brief Controls the stochastic growth behaviour of the city.
     */
    struct growth_config_t {
        /** @brief Base probability threshold. Closer to 1.0 means slower growth. */
        float p_base = 0.990f;

        /**
         * @brief Added to the threshold per neighboring claimed tile.
         *
         * Positive values make heavily-surrounded tiles harder to absorb,
         * producing spiky or irregular city shapes. Negative values do the
         * opposite.
         */
        float bias = -0.01f;

        /**
         * @brief Added to the threshold per tile of city area.
         *
         * Positive values slow growth as the city gets larger (diminishing
         * returns). Negative values accelerate growth with size (runaway
         * expansion). Zero disables the area term entirely.
         */
        float area_bias = 1E-4;
    };

    /**
     * @brief Construction parameters for a City.
     */
    struct config_t {
        /** @brief Display name of the city. */
        const char *name;

        /**
         * @brief Seed tile in axial coordinates (q, r).
         *
         * The city is initialised with this single tile claimed and expands
         * outward from it each growth tick.
         */
        hex_coord_t location;

        /** @brief Growth behaviour parameters. */
        growth_config_t growth;
    };

    City();
    ~City();

    /**
     * @brief Constructs a City and claims its seed tile.
     * @param config Construction parameters.
     */
    City(config_t config);

    /**
     * @brief Returns the construction parameters for this city.
     * @return A const reference to the city's config_t.
     */
    const config_t &GetConfig() const;

    /**
     * @brief Returns the set of hex tiles currently claimed by this city.
     * @return A const reference to the set of claimed hex_coord_t tiles.
     */
    const std::set<hex_coord_t> &GetTiles() const;

    /**
     * @brief Claims a tile on behalf of this city.
     *
     * Called by Region during the commit phase of each growth tick.
     *
     * @param tile The tile to absorb into this city.
     */
    void AbsorbTile(hex_coord_t tile);

    /**
     * @brief Returns the set of unclaimed tiles adjacent to this city that lie
     *        within the given region boundary.
     *
     * @param region_tiles The set of tiles that belong to the parent region.
     * @return A vector of candidate tiles eligible for growth this tick.
     */
    std::vector<hex_coord_t> GetFrontier(const std::set<hex_coord_t> &region_tiles) const;

    /**
     * @brief Computes the growth threshold for a candidate tile.
     *
     * Threshold = clamp(p_base + bias * n + area_bias * A, 0, 1), where n is
     * the number of this city's already-claimed tiles that neighbor the
     * candidate tile and A is the total area of the city in tiles.
     * A city's roll must exceed this value for it to claim the tile.
     *
     * @param tile The candidate tile to evaluate.
     * @return The threshold value in [0, 1].
     */
    float ComputeThreshold(hex_coord_t tile) const;

    /**
     * @brief Per-tick update. Reserved for future city-level simulation logic.
     */
    void Service();

  protected:
  private:
    config_t              m_config;
    std::set<hex_coord_t> m_tiles;
};

} // namespace CityPlanner
