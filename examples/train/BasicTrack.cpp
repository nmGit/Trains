#include "BasicTrack.h"

#include "CityPlanner/Rail/RailNetworkBuilder.h"
#include "CityPlanner/Terrain/Terrain.h"
#include "CityPlanner/World.h"
#include "Drafter/Canvas.h"
#include "Drafter/Overlay/DebugOverlay.h"
#include "Log/Log.h"
#include "interface/worldview/WorldView.h"

namespace Examples {

static LogContext logContext("Examples::BasicTrack");

void RunBasicTrack() {
    Log::Info(logContext, "Starting BasicTrack example");

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

    // --- Build a rail network centered in the map with the RailNetworkBuilder ---
    // Side convention (clockwise from top): 0=up, 1=upper-right, 2=lower-right,
    //                                       3=down, 4=lower-left, 5=upper-left
    //
    // Start at axial {20, -2} (visual center of 50x20 world), heading upper-right (side 1).
    //
    // The network consists of two nested circuits sharing junctions at {20,-2} and {23,-7}:
    //
    //   Inner teardrop  — {20,-2} → upper arc → {23,-7} → lower arc → {20,-2}
    //   Outer circuit   — {20,-2} → northeast tail → {35,-12} → upper-left return arc
    //                     → {23,-7} → lower arc → {20,-2}
    //
    // The teardrop loop (steps 1–12) closes at the start tile.  Step 13 departs the
    // {20,-2} junction onto the northeast tail, which snakes to {35,-12} (step 31).
    // Steps 32–44 form the return arc: a right-hand turn carries the track up and
    // upper-left above the teardrop, a second right-hand turn at {29,-13} bends it
    // lower-left, and six straights descend to re-enter the teardrop at {23,-7},
    // closing the outer circuit.
    //
    // Tile bounds: q ∈ [19, 35], offset_r ∈ [1, 8] — well clear of every world boundary.
    CityPlanner::RailNetworkBuilder builder;
    auto cur = builder.Start({20, -2}, 1);

    // --- Teardrop loop (steps 1–12, closes back at the start tile) ---
    cur = builder.BuildStraight(cur);    // {21,-3}  exit 1
    cur = builder.BuildStraight(cur);    // {22,-4}  exit 1
    cur = builder.BuildStraight(cur);    // {23,-5}  exit 1
    cur = builder.BuildCurveRight(cur);  // {24,-6}  exit 0
    cur = builder.BuildCurveRight(cur);  // {24,-7}  exit 5
    cur = builder.BuildCurveRight(cur);  // {23,-7}  exit 4
    cur = builder.BuildStraight(cur);    // {22,-6}  exit 4
    cur = builder.BuildStraight(cur);    // {21,-5}  exit 4
    cur = builder.BuildStraight(cur);    // {20,-4}  exit 4
    cur = builder.BuildCurveRight(cur);  // {19,-3}  exit 3
    cur = builder.BuildCurveRight(cur);  // {19,-2}  exit 2
    cur = builder.BuildCurveRight(cur);  // {20,-2}  exit 1  ← closes loop, junction at start tile

    // --- Northeast tail (steps 13–31, departs the junction via a left curve) ---
    cur = builder.BuildCurveLeft(cur);   // {21,-3}  exit 2  ← junction at shared tile
    cur = builder.BuildStraight(cur);    // {22,-3}  exit 2
    cur = builder.BuildStraight(cur);    // {23,-3}  exit 2
    cur = builder.BuildStraight(cur);    // {24,-3}  exit 2
    cur = builder.BuildCurveRight(cur);  // {25,-3}  exit 1
    cur = builder.BuildCurveRight(cur);  // {26,-4}  exit 0
    cur = builder.BuildStraight(cur);    // {26,-5}  exit 0
    cur = builder.BuildStraight(cur);    // {26,-6}  exit 0
    cur = builder.BuildCurveLeft(cur);   // {26,-7}  exit 1
    cur = builder.BuildCurveLeft(cur);   // {27,-8}  exit 2
    cur = builder.BuildStraight(cur);    // {28,-8}  exit 2
    cur = builder.BuildCurveRight(cur);  // {29,-8}  exit 1
    cur = builder.BuildCurveRight(cur);  // {30,-9}  exit 0
    cur = builder.BuildCurveLeft(cur);   // {30,-10} exit 1
    cur = builder.BuildCurveLeft(cur);   // {31,-11} exit 2
    cur = builder.BuildStraight(cur);    // {32,-11} exit 2
    cur = builder.BuildStraight(cur);    // {33,-11} exit 2
    cur = builder.BuildCurveRight(cur);  // {34,-11} exit 1
    cur = builder.BuildCurveRight(cur);  // {35,-12} exit 0

    // --- Return arc (steps 32–44, sweeps upper-left back to the teardrop) ---
    cur = builder.BuildCurveRight(cur);  // {35,-13} exit 5
    cur = builder.BuildStraight(cur);    // {34,-13} exit 5
    cur = builder.BuildStraight(cur);    // {33,-13} exit 5
    cur = builder.BuildStraight(cur);    // {32,-13} exit 5
    cur = builder.BuildStraight(cur);    // {31,-13} exit 5
    cur = builder.BuildStraight(cur);    // {30,-13} exit 5
    cur = builder.BuildCurveRight(cur);  // {29,-13} exit 4
    cur = builder.BuildStraight(cur);    // {28,-12} exit 4
    cur = builder.BuildStraight(cur);    // {27,-11} exit 4
    cur = builder.BuildStraight(cur);    // {26,-10} exit 4
    cur = builder.BuildStraight(cur);    // {25,-9}  exit 4
    cur = builder.BuildStraight(cur);    // {24,-8}  exit 4
    cur = builder.BuildStraight(cur);    // {23,-7}  entry 1, exit 4  ← closes circuit, junction at teardrop

    CityPlanner::RailNetwork network = builder.Build();
    worldview.SetNetwork(network);
    worldview.SetBuilder(&builder);
    worldview.SetTrackCursor(cur);

    while (true) {
        world.Service();
        if (canvas.Service() == Drafter::Canvas::ServiceResult::Quit) break;
        worldview.Service();
        canvas.Present();
    }
}

} // namespace Examples
