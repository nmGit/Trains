#include "WorldView.h"

#include "CityPlanner/Utils.h"
#include "Drafter/Utils/Color.h"
#include "Log/Log.h"

#include <cmath>
#include <numbers>

namespace Trains {

WorldView::WorldView(CityPlanner::World &world, Drafter::Canvas &canvas)
    : m_canvas(canvas), m_world(world) {}

void WorldView::Start() {
    m_world.OnRegionAdded().Subscribe(this, &WorldView::SlotRegionAdded);

    m_camera.Attach(m_canvas);

    // --- Compute world pixel bounds and set camera limits ---
    const auto &wcfg = m_world.GetConfig();
    if (wcfg.width > 0 && wcfg.height > 0) {
        auto p_min = CityPlanner::AxialToPixel({0, 0});
        auto p_max = CityPlanner::AxialToPixel({wcfg.width - 1, wcfg.height - 1});
        float pad  = k_cell_radius * 2.f;
        Drafter::bounds_t wb{
            .x = p_min.x * k_cell_radius - pad,
            .y = p_min.y * k_cell_radius - pad,
            .w = (p_max.x - p_min.x) * k_cell_radius + pad * 2.f,
            .h = (p_max.y - p_min.y) * k_cell_radius + pad * 2.f,
        };
        m_camera.SetBounds(wb);
    }

    m_hex_grid.emplace(Drafter::HexGrid::config_t{
        .cell_radius  = k_cell_radius,
        .stroke_color = BLRgba32(0xFF474747),
        .stroke_width = 1.f,
        .stroke_fade  = {.fade_start = 0.8f, .fade_end = 0.4f},
    });
}

void WorldView::Service() {
    BLContext             &ctx    = m_canvas.GetRenderer();
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

    // 2. Terrain (rivers, etc.) — fill only, no borders.
    DrawTerrain(ctx, params);

    // 3. Cities
    for (auto &city_view : m_city_views) {
        city_view->Service(params);
    }
}

void WorldView::DrawTerrain(BLContext &ctx, const Drafter::draw_params_t &params) {
    const auto &tiles = m_world.GetTileMap();
    if (tiles.empty()) return;

    const Drafter::bounds_t &vb = params.view_bounds;

    constexpr float k_step     = 2.0f * std::numbers::pi_v<float> / 6.0f;
    constexpr float k_max_water = 10.f; // matches terrain service default
    const float     r           = k_cell_radius;

    // Base river color (full opacity variant — alpha set per-tile).
    static const Drafter::Color::hsv_t k_river_hsv{210.f, 0.75f, 0.50f, 255};
    // Base forest color.
    static const Drafter::Color::hsv_t k_forest_hsv{120.f, 0.50f, 0.45f, 255};
    constexpr float k_max_wood = 10.f;

    // Base dirt colors: gray (low score) to rich brown (high score).
    static const Drafter::Color::hsv_t k_dirt_lo{30.f, 0.05f, 0.28f, 255}; // gray
    static const Drafter::Color::hsv_t k_dirt_hi{25.f, 0.65f, 0.40f, 255}; // rich brown

    ctx.save();

    // --- Dirt tiles (brown-ness driven by dirt_score) ---
    for (const auto &[coord, props] : tiles) {
        if (props.dirt_score <= 0.f || props.is_river) continue;

        auto  p  = CityPlanner::AxialToPixel(coord);
        float cx = p.x * r;
        float cy = p.y * r;

        if (!vb.Intersects(cx, cy, r)) continue;

        float t = std::clamp(props.dirt_score, 0.f, 1.f);
        Drafter::Color::hsv_t hsv{
            k_dirt_lo.h + t * (k_dirt_hi.h - k_dirt_lo.h),
            k_dirt_lo.s + t * (k_dirt_hi.s - k_dirt_lo.s),
            k_dirt_lo.v + t * (k_dirt_hi.v - k_dirt_lo.v),
            255};
        BLRgba32 color = Drafter::Color::HsvToBL(hsv);

        ctx.set_fill_style(color);

        BLPath hex;
        hex.move_to(cx + r * std::cos(0.f), cy + r * std::sin(0.f));
        for (int v = 1; v < 6; ++v) {
            float a = k_step * static_cast<float>(v);
            hex.line_to(cx + r * std::cos(a), cy + r * std::sin(a));
        }
        hex.close();
        ctx.fill_path(hex);
    }

    // --- River tiles (blue, water-driven opacity) ---
    for (const auto &[coord, props] : tiles) {
        if (!props.is_river) continue;

        auto  p  = CityPlanner::AxialToPixel(coord);
        float cx = p.x * r;
        float cy = p.y * r;

        if (!vb.Intersects(cx, cy, r)) continue;

        // Opacity driven by water level: [0, max_water] → [15, 220].
        float t     = std::clamp(props.water / k_max_water, 0.f, 1.f);
        auto  alpha = static_cast<uint8_t>(15.f + t * 205.f);
        BLRgba32 color = Drafter::Color::HsvToBL(
            {k_river_hsv.h, k_river_hsv.s, k_river_hsv.v, alpha});

        ctx.set_fill_style(color);

        BLPath hex;
        hex.move_to(cx + r * std::cos(0.f), cy + r * std::sin(0.f));
        for (int v = 1; v < 6; ++v) {
            float a = k_step * static_cast<float>(v);
            hex.line_to(cx + r * std::cos(a), cy + r * std::sin(a));
        }
        hex.close();
        ctx.fill_path(hex);
    }

    // --- Forest tiles (green, wood-driven opacity) ---
    for (const auto &[coord, props] : tiles) {
        if (!props.is_forest) continue;

        auto  p  = CityPlanner::AxialToPixel(coord);
        float cx = p.x * r;
        float cy = p.y * r;

        if (!vb.Intersects(cx, cy, r)) continue;

        float t     = std::clamp(props.wood / k_max_wood, 0.f, 1.f);
        auto  alpha = static_cast<uint8_t>(40.f + t * 180.f);
        BLRgba32 color = Drafter::Color::HsvToBL(
            {k_forest_hsv.h, k_forest_hsv.s, k_forest_hsv.v, alpha});

        ctx.set_fill_style(color);

        BLPath hex;
        hex.move_to(cx + r * std::cos(0.f), cy + r * std::sin(0.f));
        for (int v = 1; v < 6; ++v) {
            float a = k_step * static_cast<float>(v);
            hex.line_to(cx + r * std::cos(a), cy + r * std::sin(a));
        }
        hex.close();
        ctx.fill_path(hex);
    }

    ctx.restore();
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
