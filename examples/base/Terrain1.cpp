#include "Terrain1.h"

#include "CityPlanner/Terrain/Terrain.h"
#include "CityPlanner/World.h"
#include "Drafter/Canvas.h"
#include "Drafter/Overlay/DataTreeOverlay.h"
#include "Drafter/Overlay/DebugOverlay.h"
#include "Log/Log.h"
#include "interface/worldview/WorldView.h"

namespace Examples {
static LogContext logContext("Examples::Terrain1");

void RunTerrain1() {
    Log::Info(logContext, "Starting Terrain1 example");

    Drafter::Canvas    canvas;
    CityPlanner::World world({.width = 80, .height = 80});
    Trains::WorldView  worldview(world, canvas);

    // Generate terrain (dirt → rivers → forests).
    CityPlanner::TerrainGenerator::Generate({
        .seed    = 12345,
        .dirt    = {.num_seeds = 20, .noise = 0.03f},
        .rivers  = {.num_rivers = 4, .base_width = 3},
        .forests = {.num_seeds = 12, .min_seed_dist = 8},
    }, world);

    // Enable per-tick water flow simulation.
    world.EnableTerrainService({}, 12345);

    canvas.Start();
    worldview.Start();
    Drafter::DebugOverlay debug_overlay{canvas};

    // Place a couple of cities for visual context.
    auto &reg    = world.AddRegion();
    auto &city1  = reg.AddCity(CityPlanner::City::config_t{"Riverside", {20, 20}});
    auto &city2  = reg.AddCity(CityPlanner::City::config_t{"Uptown",    {50, 40}});

    // Build city data tree — structure is fixed, leaf values update live.
    Drafter::DataTreeOverlay city_tree{canvas, "Cities"};

    for (auto *city : {&city1, &city2}) {
        auto *branch = city_tree.CreateBranch(city->GetConfig().name);
        branch->AddLeaf("Size (tiles)", [city]{ return city->GetTiles().size(); });
        branch->AddLeaf("Water",        [city]{ return city->GetRawResources().water; });
        branch->AddLeaf("Wood",         [city]{ return city->GetRawResources().wood; });
        branch->AddLeaf("Dirt",         [city]{ return city->GetRawResources().dirt; });
    }

    while (true) {
        world.Service();
        if (canvas.Service() == Drafter::Canvas::ServiceResult::Quit) break;
        worldview.Service();
        canvas.Present();
    }
}
} // namespace Examples
