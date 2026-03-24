#include "WorldView.h"

#include "Log/Log.h"

#include <cmath>

namespace Trains {

WorldView::WorldView(CityPlanner::World &world, Drafter::Canvas &canvas)
    : m_canvas(canvas), m_world(world) {}

void WorldView::Start() {
    m_world.OnRegionAdded().Subscribe(this, &WorldView::SlotRegionAdded);

    m_camera.Attach(m_canvas);

    m_hex_grid.emplace(Drafter::HexGrid::config_t{
        .cell_radius  = k_cell_radius,
        .stroke_color = BLRgba32(0xFF474747),
        .stroke_width = 1.f,
        .stroke_fade  = {.fade_start = 0.8f, .fade_end = 0.4f},
    });
}

void WorldView::Service() {
    BLContext            &ctx    = m_canvas.GetRenderer();
    Drafter::draw_params_t params = m_canvas.GetDrawParams();

    // 1. Background hex grid — snap to nearest even-column boundary so the
    //    pattern tiles seamlessly as the camera pans.
    if (m_hex_grid.has_value()) {
        const float col_step = 1.5f * k_cell_radius;
        const float row_step = k_cell_radius * std::sqrt(3.0f);
        const auto &vb       = params.view_bounds;

        int snap_col = static_cast<int>(std::floor(vb.x / col_step));
        if (snap_col % 2 != 0) snap_col -= 1;
        const float snap_x = static_cast<float>(snap_col) * col_step;
        const float snap_y = std::floor(vb.y / row_step) * row_step;

        m_hex_grid->SetPosition({snap_x, snap_y});
        m_hex_grid->Draw(ctx, params);
    }

    // 2. Cities
    for (auto &city_view : m_city_views) {
        city_view->Service(params);
    }
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
