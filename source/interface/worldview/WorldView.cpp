#include "WorldView.h"

#include "Types/Utils.h"
#include "Drafter/Utils/Color.h"
#include "Log/Log.h"

#include <cmath>
#include <numbers>

namespace Trains {

WorldView::WorldView(CityPlanner::World &world, Drafter::Canvas &canvas)
    : m_canvas(canvas), m_world(world) {}

void WorldView::Start() {
    m_world.OnRegionAdded().Subscribe(this, &WorldView::SlotRegionAdded);
    m_canvas.OnSDLEvent().Subscribe(this, &WorldView::SlotMouseMotion);

    m_camera.Attach(m_canvas);

    // --- Compute world pixel bounds and set camera limits ---
    const auto &wcfg = m_world.GetConfig();
    if (wcfg.width > 0 && wcfg.height > 0) {
        // The world is defined in even-q offset space: [0,width) x [0,height).
        // Convert the four offset-space corners to axial, then to pixel to get
        // the true bounding box.
        const int max_q = wcfg.width - 1;
        const int max_or = wcfg.height - 1;
        auto p00 = Types::AxialToPixel({0,     0});
        auto p10 = Types::AxialToPixel({max_q, -max_q / 2});
        auto p01 = Types::AxialToPixel({0,     max_or});
        auto p11 = Types::AxialToPixel({max_q, max_or - max_q / 2});

        float x_min = std::min({p00.x, p10.x, p01.x, p11.x}) * k_cell_radius;
        float x_max = std::max({p00.x, p10.x, p01.x, p11.x}) * k_cell_radius;
        float y_min = std::min({p00.y, p10.y, p01.y, p11.y}) * k_cell_radius;
        float y_max = std::max({p00.y, p10.y, p01.y, p11.y}) * k_cell_radius;

        float pad = k_cell_radius * 2.f;
        Drafter::bounds_t wb{
            .x = x_min - pad,
            .y = y_min - pad,
            .w = (x_max - x_min) + pad * 2.f,
            .h = (y_max - y_min) + pad * 2.f,
        };
        m_camera.SetBounds(wb);
    }

    m_hex_grid.emplace(Drafter::HexGrid::config_t{
        .cell_radius  = k_cell_radius,
        .stroke_color = BLRgba32(0xFF474747),
        .stroke_width = 1.f,
        .stroke_fade  = {.fade_start = 1.9f, .fade_end = 0.9f},
    });
}

void WorldView::Service() {
    BLContext             &ctx    = m_canvas.GetRenderer();
    Drafter::draw_params_t params = m_canvas.GetDrawParams();

    DrawHexGrid(ctx, params);
    DrawTerrain(ctx, params);
    DrawCities(ctx, params);
    DrawTransport(params);
    DrawHoveredTile(ctx, params);
}

void WorldView::DrawHexGrid(BLContext &ctx, const Drafter::draw_params_t &params) {
    if (!m_hex_grid.has_value()) return;

    // Snap to nearest even-column boundary so the pattern tiles seamlessly.
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

        auto  p  = Types::AxialToPixel(coord);
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

        auto  p  = Types::AxialToPixel(coord);
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

        auto  p  = Types::AxialToPixel(coord);
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

void WorldView::SetNetwork(const CityPlanner::RailNetwork &network) {
    m_transport_view.emplace(network, m_canvas, k_cell_radius);
}

void WorldView::SetBuilder(const CityPlanner::RailNetworkBuilder *builder) {
    m_builder = builder;
    if (!m_builder && m_transport_view) {
        m_transport_view->ClearGhostPath();
    }
}

void WorldView::SetTrackCursor(
    std::optional<CityPlanner::RailNetwork::segment_t> cursor) {
    m_track_cursor = cursor;
}

void WorldView::DrawTransport(const Drafter::draw_params_t &params) {
    if (!m_transport_view.has_value()) return;
    m_transport_view->Service(params);
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

void WorldView::DrawCities(BLContext &, const Drafter::draw_params_t &params) {
    for (auto &city_view : m_city_views) {
        city_view->Service(params);
    }
}

void WorldView::DrawHoveredTile(BLContext &ctx, const Drafter::draw_params_t &) {
    if (!m_hovered_tile || !m_world.InBounds(*m_hovered_tile)) return;

    constexpr float k_step = 2.0f * std::numbers::pi_v<float> / 6.0f;
    auto  p  = Types::AxialToPixel(*m_hovered_tile);
    float cx = p.x * k_cell_radius;
    float cy = p.y * k_cell_radius;

    ctx.save();
    ctx.set_fill_style(BLRgba32(0x55FFFFFF)); // semi-transparent white

    BLPath hex;
    hex.move_to(cx + k_cell_radius * std::cos(0.f),
                cy + k_cell_radius * std::sin(0.f));
    for (int v = 1; v < 6; ++v) {
        float a = k_step * static_cast<float>(v);
        hex.line_to(cx + k_cell_radius * std::cos(a),
                    cy + k_cell_radius * std::sin(a));
    }
    hex.close();
    ctx.fill_path(hex);
    ctx.restore();
}

void WorldView::SlotMouseMotion(const SDL_Event &event) {
    if (event.type != SDL_EVENT_MOUSE_MOTION) return;

    // Convert screen-space mouse position to world space, then to hex coords.
    // AxialToPixel uses hex_size=1; world space is scaled by k_cell_radius.
    Drafter::point_t world = m_camera.ScreenToWorld(
        {event.motion.x, event.motion.y}, m_canvas.GetGeometry());
    m_hovered_tile = Types::PixelToAxial(
        {world.x / k_cell_radius, world.y / k_cell_radius});

    if (m_builder && m_track_cursor && m_hovered_tile && m_transport_view) {
        auto ghost = m_builder->Preview(*m_track_cursor, *m_hovered_tile, m_world);
        if (ghost) {
            m_transport_view->SetGhostPath(std::move(*ghost));
        } else {
            m_transport_view->ClearGhostPath();
        }
    }
}

} // namespace Trains
