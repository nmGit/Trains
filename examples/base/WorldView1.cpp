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

    CityPlanner::City::config_t cityConfig{"New City", {10, 5}};
    CityPlanner::City::config_t city2Config{"New City2", {10, 15}};
    CityPlanner::City::config_t city3Config{"New City3", {40, 5}};

    // Start the canvas
    canvas.Start();
    worldview.Start();
    auto& reg = world.AddRegion();
    reg.AddCity(cityConfig);
    reg.AddCity(city2Config);
    reg.AddCity(city3Config);

    while (1) {
        SDL_Delay(10);
        // Advance the simulation
        world.Service();
        // Redraw the world
        worldview.Service();
    }
}
} // namespace Examples