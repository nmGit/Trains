#include "WorldView.h"

#include "Log/Log.h"

namespace Trains {

WorldView::WorldView(CityPlanner::World &world, Drafter::Canvas &canvas)
    : m_canvas(canvas), m_world(world) {}

void WorldView::Start() {
    m_world.OnRegionAdded().Subscribe(this, &WorldView::SlotRegionAdded);

    const auto &geo = m_canvas.GetGeometry();

    m_hex_grid.emplace(Drafter::HexGrid::config_t{
        .cell_radius  = k_cell_radius,
        .bounds_w     = geo.size.w,
        .bounds_h     = geo.size.h,
        .stroke_color = BLRgba32(0xFF474747),
        .stroke_width = 1.f,
    });
}

void WorldView::Service() {
    BLContext &ctx = m_canvas.GetRenderer();

    // 1. Background hex grid
    if (m_hex_grid.has_value()) {
        m_hex_grid->Draw(ctx);
    }

    // 2. Cities (fill + border each frame so growth is visible)
    for (auto &city_view : m_city_views) {
        city_view->Service();
    }

    // 3. Flip to screen
    m_canvas.Service();
}

void WorldView::SlotRegionAdded(CityPlanner::Region &region) {
    Log::Info(m_log_context, "Region added to WorldView");
    region.OnCityAdded().Subscribe(this, &WorldView::SlotCityAdded);
}

void WorldView::SlotCityAdded(CityPlanner::City &city) {
    Log::Info(m_log_context, "City added to WorldView");
    m_city_views.push_back(
        std::make_unique<CityView>(&city, m_canvas, k_cell_radius));
}

} // namespace Trains
