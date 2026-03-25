#pragma once
#include "ProductionCommon.h"

#include <initializer_list>
#include <utility>

namespace CitySim {

static constexpr int k_num_resources = static_cast<int>(ResourceType::kCount);

/**
 * @brief Describes the inputs required to produce a batch of a resource.
 *
 * @p output.q units of @p output.r are produced per run.
 * @p inputs is a flat array indexed by ResourceType; a zero entry means the
 * resource is not required.  Direct array indexing gives O(1) stock checks
 * with no heap allocation.
 */
struct Recipe {
    struct {
        ResourceType r;
        int          q;
    } output;
    int inputs[k_num_resources];

    constexpr Recipe(ResourceType out_r, int out_q,
                     std::initializer_list<std::pair<ResourceType, int>> in)
        : output{out_r, out_q}, inputs{}
    {
        for (auto [r, q] : in)
            inputs[static_cast<int>(r)] = q;
    }
};

/**
 * @brief The full set of production recipes for every manufactured resource.
 *
 * Raw goods (Wheat, Wood, Stone, Cotton) are harvested directly and have no
 * recipe.  All other resources require inputs drawn from lower tiers.
 */
inline constexpr Recipe k_recipes[] = {

    // -------------------------------------------------------------------------
    // Basic Goods
    // -------------------------------------------------------------------------
    { ResourceType::kBread,      10, {{ ResourceType::kWheat,  1 }} },
    { ResourceType::kPaper,     100, {{ ResourceType::kWood,   1 }} },
    { ResourceType::kFurniture,   3, {{ ResourceType::kWood,   1 }} },
    { ResourceType::kClothing,    5, {{ ResourceType::kCotton, 1 }} },
    { ResourceType::kHandTools,   2, {{ ResourceType::kWood,   1 },
                                       { ResourceType::kStone,  1 }} },

    // -------------------------------------------------------------------------
    // Industrial Goods
    // -------------------------------------------------------------------------
    { ResourceType::kIron,       10, {{ ResourceType::kStone,     2 }} },
    { ResourceType::kTools,       3, {{ ResourceType::kIron,      2 },
                                       { ResourceType::kHandTools, 1 }} },
    { ResourceType::kChemicals,  20, {{ ResourceType::kWood,  1 },
                                       { ResourceType::kStone, 1 }} },

    // -------------------------------------------------------------------------
    // Technology Goods
    // -------------------------------------------------------------------------
    { ResourceType::kCircuits,    5, {{ ResourceType::kIron,      1 },
                                       { ResourceType::kChemicals, 1 }} },
    { ResourceType::kMicrochips,  3, {{ ResourceType::kCircuits,  2 },
                                       { ResourceType::kChemicals, 1 }} },
    { ResourceType::kProcessors,  1, {{ ResourceType::kMicrochips, 2 },
                                       { ResourceType::kCircuits,  1 }} },

    // -------------------------------------------------------------------------
    // Automation Goods
    // -------------------------------------------------------------------------
    { ResourceType::kRobots,        1, {{ ResourceType::kProcessors, 2 },
                                         { ResourceType::kTools,      2 }} },
    { ResourceType::kVisionSystems, 1, {{ ResourceType::kProcessors, 1 },
                                         { ResourceType::kCircuits,   2 }} },
    { ResourceType::kCncMachines,   1, {{ ResourceType::kTools,      2 },
                                         { ResourceType::kProcessors, 1 }} },
    { ResourceType::kPrinters3D,    1, {{ ResourceType::kProcessors, 1 },
                                         { ResourceType::kIron,       2 }} },
    { ResourceType::kLaserCutters,  1, {{ ResourceType::kCircuits,   1 },
                                         { ResourceType::kChemicals,  1 }} },
    { ResourceType::kServerFarms,   1, {{ ResourceType::kProcessors, 3 },
                                         { ResourceType::kIron,       1 }} },

    // -------------------------------------------------------------------------
    // Transportation Goods
    // -------------------------------------------------------------------------
    { ResourceType::kTrains,  1, {{ ResourceType::kIron,  3 },
                                   { ResourceType::kTools, 1 }} },
    { ResourceType::kTracks, 10, {{ ResourceType::kIron,  4 },
                                   { ResourceType::kStone, 1 }} },
};

} // namespace CitySim
