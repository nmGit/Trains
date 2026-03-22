#include "CityView.h"

#include "CityPlanner/Utils.h"

#include <algorithm>
#include <blend2d/blend2d.h>
#include <cmath>
#include <numbers>
#include <random>

namespace Trains {

namespace {

/// Converts HSV (h in [0,360), s/v in [0,1]) to a packed 0xAARRGGBB color.
BLRgba32 HsvToRgb(float h, float s, float v, uint8_t alpha = 255) {
    float c  = v * s;
    float x  = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m  = v - c;
    float r, g, b;
    if      (h < 60)  { r=c; g=x; b=0; }
    else if (h < 120) { r=x; g=c; b=0; }
    else if (h < 180) { r=0; g=c; b=x; }
    else if (h < 240) { r=0; g=x; b=c; }
    else if (h < 300) { r=x; g=0; b=c; }
    else              { r=c; g=0; b=x; }
    return BLRgba32(
        static_cast<uint8_t>((r + m) * 255),
        static_cast<uint8_t>((g + m) * 255),
        static_cast<uint8_t>((b + m) * 255),
        alpha);
}

float RandomHue() {
    static std::mt19937                          rng{std::random_device{}()};
    static std::uniform_real_distribution<float> dist{0.0f, 360.0f};
    return dist(rng);
}

// Maps edge index e (edge from vertex e to vertex (e+1)%6) to the index into
// CityPlanner::Neighbors() that faces that edge for a flat-top axial hex.
// Derived from the outward midpoint direction of each edge (y increases down):
//
//  Edge 0 (v0→v1) : lower-right  (q+1, r  ) → Neighbors[0]
//  Edge 1 (v1→v2) : down         (q,   r+1) → Neighbors[2]
//  Edge 2 (v2→v3) : lower-left   (q-1, r+1) → Neighbors[5]
//  Edge 3 (v3→v4) : upper-left   (q-1, r  ) → Neighbors[1]
//  Edge 4 (v4→v5) : up           (q,   r-1) → Neighbors[3]
//  Edge 5 (v5→v0) : upper-right  (q+1, r-1) → Neighbors[4]
constexpr int k_edge_to_neighbor[6] = {0, 2, 5, 1, 3, 4};

} // namespace

CityView::CityView(CityPlanner::City *city, Drafter::Canvas &canvas, float cell_radius)
    : m_city(city), m_canvas(canvas), m_cell_radius(cell_radius) {
    float hue     = RandomHue();
    m_border_color = HsvToRgb(hue, 0.90f, 1.00f, 255);
    m_fill_color   = HsvToRgb(hue, 0.70f, 0.45f, 180);
}

BLPoint CityView::TileCenter(CityPlanner::hex_coord_t tile) const {
    // AxialToPixel returns coordinates at hex_size=1; scale by cell_radius.
    auto p = CityPlanner::AxialToPixel(tile);
    return {static_cast<double>(p.x * m_cell_radius),
            static_cast<double>(p.y * m_cell_radius)};
}

void CityView::Service() {
    const auto &tiles = m_city->GetTiles();
    if (tiles.empty()) return;

    BLContext &ctx = m_canvas.GetRenderer();

    constexpr float k_two_pi    = 2.0f * std::numbers::pi_v<float>;
    constexpr float k_step      = k_two_pi / 6.0f;
    const     float r           = m_cell_radius;

    // --- Fill pass ---
    // Fill each claimed tile as a solid hexagon with the semi-transparent color.
    // Drawing tile-by-tile lets Blend2D composite them naturally.
    ctx.save();
    ctx.set_fill_style(m_fill_color);

    for (const auto &tile : tiles) {
        auto [cx, cy] = TileCenter(tile);
        BLPath hex;
        hex.move_to(cx + r * std::cos(0.0f), cy + r * std::sin(0.0f));
        for (int v = 1; v < 6; ++v) {
            float a = k_step * static_cast<float>(v);
            hex.line_to(cx + r * std::cos(a), cy + r * std::sin(a));
        }
        hex.close();
        ctx.fill_path(hex);
    }

    ctx.restore();

    // --- Border pass ---
    // Stroke only edges where the neighboring tile is not part of this city.
    // This produces a clean outer boundary without internal dividing lines.
    ctx.save();
    ctx.set_stroke_style(m_border_color);
    ctx.set_stroke_width(static_cast<double>(r * 0.25f));
    ctx.set_stroke_join(BL_STROKE_JOIN_ROUND);
    ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);

    BLPath border;
    for (const auto &tile : tiles) {
        auto [cx, cy]  = TileCenter(tile);
        auto  neighbors = CityPlanner::Neighbors(tile);

        for (int e = 0; e < 6; ++e) {
            if (tiles.count(neighbors[k_edge_to_neighbor[e]])) continue;

            float a0 = k_step * static_cast<float>(e);
            float a1 = k_step * static_cast<float>(e + 1);
            border.move_to(cx + r * std::cos(a0), cy + r * std::sin(a0));
            border.line_to(cx + r * std::cos(a1), cy + r * std::sin(a1));
        }
    }

    ctx.stroke_path(border);
    ctx.restore();
}

} // namespace Trains
