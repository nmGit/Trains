#include "BasicPieMenu.h"

#include "CityPlanner/Terrain/Terrain.h"
#include "CityPlanner/World.h"
#include "Drafter/Canvas.h"
#include "Drafter/Overlay/DebugOverlay.h"
#include "Log/Log.h"
#include "interface/interaction/Interface.h"
#include "interface/worldview/WorldView.h"

#include <cstdio>

namespace Examples {

static LogContext logContext("Examples::BasicPieMenu");

void RunBasicPieMenu() {
    Log::Info(logContext, "Starting BasicPieMenu example");

    Drafter::Canvas    canvas;
    CityPlanner::World world({.width = 50, .height = 20});
    Trains::WorldView  worldview(world, canvas);

    CityPlanner::TerrainGenerator::Generate({
        .seed    = 42,
        .dirt    = {.num_seeds = 20, .noise = 0.03f},
        .rivers  = {.num_rivers = 3, .base_width = 2},
        .forests = {.num_seeds = 10, .min_seed_dist = 8},
    }, world);

    world.EnableTerrainService({}, 42);

    canvas.Start();
    worldview.Start();
    Drafter::DebugOverlay debug_overlay{canvas};

    Trains::Interface interface{canvas};

    // Segoe UI ships with Windows; substitute any TTF/OTF path on other platforms.
    interface.GetPieMenu().SetFont("C:/Windows/Fonts/segoeui.ttf", 13.f);

    interface.GetPieMenu().AddOption("Build Station", [] {
        printf("[PieMenu] Build Station selected\n");
    });
    interface.GetPieMenu().AddOption("Build Track", [] {
        printf("[PieMenu] Build Track selected\n");
    });
    interface.GetPieMenu().AddOption("Inspect Tile", [] {
        printf("[PieMenu] Inspect Tile selected\n");
    });
    interface.GetPieMenu().AddOption("Remove", [] {
        printf("[PieMenu] Remove selected\n");
    });

    while (true) {
        world.Service();
        if (canvas.Service() == Drafter::Canvas::ServiceResult::Quit) break;
        worldview.Service();
        canvas.Present();
    }
}

} // namespace Examples
