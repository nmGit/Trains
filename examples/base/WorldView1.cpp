#include "WorldView1.h"

#include "CityPlanner/World.h"
#include "Drafter/Canvas.h"
#include "Log/Log.h"
#include "interface/worldview/WorldView.h"

#include <cstdio>

namespace Examples {
static LogContext logContext("Examples::WorldView1");

void RunWorldView1() {
    Log::Info(logContext, "Starting WorldView1 example");
    Drafter::Canvas    canvas;
    CityPlanner::World world;
    Trains::WorldView  worldview(world, canvas);

    CityPlanner::City::config_t cityConfig{"New City", {100, 100}};

    // Start the canvas
    canvas.Start();
    worldview.Start();

    world.AddRegion().AddCity(cityConfig);

    while (1) {
        SDL_Delay(100);
        worldview.Service();
        canvas.Service();
    }
}
} // namespace Examples