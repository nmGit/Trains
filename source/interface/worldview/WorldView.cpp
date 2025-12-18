#include "WorldView.h"

#include "Drafter/Shapes/Circle.h"

namespace Trains {
WorldView::WorldView(CityPlanner::World &world, Drafter::Canvas &canvas)
    : m_canvas(canvas), m_world(world) {}

void WorldView::Service() {}

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

    Drafter::Circle::config_t          config{
                 .position{.x = city_config.location.x, .y = city_config.location.y},
                 .radius = 25.f};

    new Drafter::Circle(m_canvas, config);
}

} // namespace Trains
