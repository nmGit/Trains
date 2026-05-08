#include "BasicMenu.h"

#include "CityPlanner/Terrain/Terrain.h"
#include "CityPlanner/World.h"
#include "Drafter/Canvas.h"
#include "Drafter/Overlay/DebugOverlay.h"
#include "Log/Log.h"
#include "interface/worldview/WorldView.h"

namespace Examples {

static LogContext logContext("Examples::BasicMenu");

void RunBasicMenu() {
    Log::Info(logContext, "Starting BasicMenu example");

    Drafter::Canvas    canvas;
    CityPlanner::World world({.width = 50, .height = 20});
    Trains::WorldView  worldview(world, canvas);

    CityPlanner::TerrainGenerator::Generate({
        .seed    = 18,
        .dirt    = {.num_seeds = 20, .noise = 0.03f},
        .rivers  = {.num_rivers = 4, .base_width = 3},
        .forests = {.num_seeds = 12, .min_seed_dist = 8},
    }, world);

    world.EnableTerrainService({}, 12345);

    canvas.Start();
    worldview.Start();
    Drafter::DebugOverlay debug_overlay{canvas};

    while (true) {
        world.Service();
        if (canvas.Service() == Drafter::Canvas::ServiceResult::Quit) break;
        worldview.Service();
        canvas.Present();
    }
}

} // namespace Examples
