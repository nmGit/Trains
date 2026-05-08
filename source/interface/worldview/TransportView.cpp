#include "TransportView.h"

#include "Types/Utils.h"

#include <blend2d/blend2d.h>
#include <cmath>
#include <numbers>

namespace Trains {

namespace {

// ---------------------------------------------------------------------------
// Hex geometry helpers
// ---------------------------------------------------------------------------

// Angle (in radians) of the outward normal for side s of a flat-top hex,
// following the RailNetworkBuilder side convention:
//   side 0 = up       (−π/2)
//   side 1 = upper-right (−π/6)
//   side 2 = lower-right (+π/6)
//   side 3 = down     (+π/2)
//   side 4 = lower-left  (+5π/6)
//   side 5 = upper-left  (−5π/6)
// Formula: s * π/3 − π/2
inline float SideAngle(int s) {
    return static_cast<float>(s) * (std::numbers::pi_v<float> / 3.0f)
           - (std::numbers::pi_v<float> / 2.0f);
}

// Midpoint of side s on a flat-top hex centered at (cx, cy) with circumradius r.
// Distance from center to edge midpoint is the apothem: r * √3/2.
inline BLPoint SideMidpoint(float cx, float cy, float r, int s) {
    float apothem = r * (std::numbers::sqrt3_v<float> / 2.0f);
    float angle   = SideAngle(s);
    return {cx + static_cast<double>(apothem * std::cos(angle)),
            cy + static_cast<double>(apothem * std::sin(angle))};
}

// Returns true when the two sides are exactly 180° apart (straight segment).
inline bool IsStraight(int entry, int exit) {
    return ((exit - entry + 6) % 6) == 3;
}

} // namespace

// ---------------------------------------------------------------------------
// TransportView
// ---------------------------------------------------------------------------

TransportView::TransportView(const CityPlanner::RailNetwork &network,
                             Drafter::Canvas &canvas, float cell_radius)
    : m_network(network), m_canvas(canvas), m_cell_radius(cell_radius) {}

void TransportView::Service(Drafter::draw_params_t params) {
    BLContext &ctx = m_canvas.GetRenderer();
    DrawTracks(ctx, params);
    DrawGhostTracks(ctx, params);
    DrawSemaphores(ctx, params);
    DrawTrains(ctx, params);
}

void TransportView::SetGhostPath(CityPlanner::RailNetwork::path_t path) {
    m_ghost_path = std::move(path);
}

void TransportView::ClearGhostPath() {
    m_ghost_path.reset();
}

void TransportView::DrawTracks(BLContext &ctx, const Drafter::draw_params_t &params) {
    const auto &segments = m_network.GetSegments();
    if (segments.empty()) return;

    const float r = m_cell_radius;

    ctx.save();
    ctx.set_stroke_style(BLRgba32(0xFFFFFFFF));
    ctx.set_stroke_width(static_cast<double>(r * 0.15f));
    ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);
    ctx.set_stroke_join(BL_STROKE_JOIN_ROUND);

    BLPath path;

    for (const auto &seg : segments) {
        auto   p  = Types::AxialToPixel(seg.hex);
        float  cx = p.x * r;
        float  cy = p.y * r;

        if (!params.view_bounds.Intersects(cx, cy, r)) continue;

        BLPoint entry_pt = SideMidpoint(cx, cy, r, seg.entry);
        BLPoint exit_pt  = SideMidpoint(cx, cy, r, seg.exit);

        if (IsStraight(seg.entry, seg.exit)) {
            path.move_to(entry_pt);
            path.line_to(exit_pt);
        } else {
            // Curved segment: quadratic Bezier with the hex center as the
            // control point, producing a gentle arc that stays inside the tile.
            path.move_to(entry_pt);
            path.quad_to(BLPoint{cx, cy}, exit_pt);
        }
    }

    ctx.stroke_path(path);
    ctx.restore();
}

void TransportView::DrawGhostTracks(BLContext &ctx, const Drafter::draw_params_t &params) {
    if (!m_ghost_path.has_value() || m_ghost_path->empty()) return;

    const float r = m_cell_radius;

    ctx.save();
    ctx.set_stroke_style(BLRgba32(0x88FFFFFF)); // white at ~53% opacity
    ctx.set_stroke_width(static_cast<double>(r * 0.15f));
    ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);
    ctx.set_stroke_join(BL_STROKE_JOIN_ROUND);

    BLPath path;

    for (const auto &seg : *m_ghost_path) {
        auto   p  = Types::AxialToPixel(seg.hex);
        float  cx = p.x * r;
        float  cy = p.y * r;

        if (!params.view_bounds.Intersects(cx, cy, r)) continue;

        BLPoint entry_pt = SideMidpoint(cx, cy, r, seg.entry);
        BLPoint exit_pt  = SideMidpoint(cx, cy, r, seg.exit);

        if (IsStraight(seg.entry, seg.exit)) {
            path.move_to(entry_pt);
            path.line_to(exit_pt);
        } else {
            path.move_to(entry_pt);
            path.quad_to(BLPoint{cx, cy}, exit_pt);
        }
    }

    ctx.stroke_path(path);
    ctx.restore();
}

void TransportView::DrawSemaphores(BLContext &, const Drafter::draw_params_t &) {}

void TransportView::DrawTrains(BLContext &, const Drafter::draw_params_t &) {}

} // namespace Trains
