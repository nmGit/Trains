#include "Terrain1.h"

#include "CityPlanner/Terrain/Terrain.h"
#include "CityPlanner/World.h"
#include "Drafter/Canvas.h"
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

    // Place a couple of cities for visual context.
    CityPlanner::City::config_t city1{"Riverside", {20, 20}};
    CityPlanner::City::config_t city2{"Uptown",    {50, 40}};
    auto &reg = world.AddRegion();
    reg.AddCity(city1);
    reg.AddCity(city2);

    while (true) {
        world.Service();
        if (canvas.Service() == Drafter::Canvas::ServiceResult::Quit) break;
        worldview.Service();
        canvas.Present();
    }
}
} // namespace Examples
