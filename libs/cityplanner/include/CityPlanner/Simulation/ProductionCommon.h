#pragma once

namespace CitySim {

/**
 * @brief Identifies every resource type in the simulation.
 *
 * Resources are grouped into tiers that reflect their position in the
 * production chain:
 *
 *  - **Raw**          — harvested directly from the land.
 *  - **Basic**        — manufactured from raw goods; nice-to-have for growth.
 *  - **Industrial**   — heavy-industry outputs; inputs for higher tiers.
 *  - **Technology**   — advanced manufactured goods.
 *  - **Automation**   — highest-tier goods; greatly boost city efficiency.
 *  - **Transportation** — special goods required to build and run the railway.
 */
enum class ResourceType {
    // --- Raw Goods ---
    kWheat,
    kWood,
    kStone,
    kCotton,

    // --- Basic Goods ---
    kBread,
    kPaper,
    kFurniture,
    kClothing,
    kHandTools,

    // --- Industrial Goods ---
    kIron,
    kTools,
    kChemicals,

    // --- Technology Goods ---
    kCircuits,
    kMicrochips,
    kProcessors,

    // --- Automation Goods ---
    kRobots,
    kVisionSystems,
    kCncMachines,
    kPrinters3D,
    kLaserCutters,
    kServerFarms,

    // --- Transportation Goods ---
    kTrains,
    kTracks,

    kCount, ///< Sentinel — total number of resource types. Keep last.
};
} // namespace CitySim