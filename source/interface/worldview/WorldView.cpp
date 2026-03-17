#include "WorldView.h"

#include "Drafter/Shapes/Circle.h"
#include "Drafter/Shapes/NGon.h"

namespace Trains {
WorldView::WorldView(CityPlanner::World &world, Drafter::Canvas &canvas)
    : m_canvas(canvas), m_world(world) {}

void WorldView::Service() {
    // Draw the background tiles
    Drafter::NGon::config_t tile_config{
        .position{.x = 0, .y = 0}, .radius = 25.f, .sides = 6};

    for (int x = 0; x < m_canvas.GetGeometry().size.w; x+=75) {

        for (int y = 0; y < m_canvas.GetGeometry().size.h; y+=45) {
            tile_config.position.x = static_cast<float>(x);
            tile_config.position.y = static_cast<float>(y);
            Drafter::NGon(m_canvas, tile_config).Draw();
        }
    }
}

void WorldView::Start() {
    m_world.OnRegionAdded().Subscribe(this, &WorldView::SlotRegionAdded);
}

void WorldView::SlotRegionAdded(CityPlanner::Region &region) {
    Log::Info(m_log_context, "Region added to WorldView");

    region.OnCityAdded().Subscribe(this, &WorldView::SlotCityAdded);
}

void WorldView::SlotCityAdded(CityPlanner::City &city) {

    Log::Info(m_log_context, "City added to WorldView");

    const CityPlanner::City::config_t &city_config = city.GetConfig();

    Drafter::Circle::config_t config{
        .position{.x = city_config.location.x, .y = city_config.location.y},
        .radius = 25.f};

    new Drafter::Circle(m_canvas, config);
}

} // namespace Trains
