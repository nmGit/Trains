#include "CityPlanner/Rail/RailNetworkBuilder.h"
#include "CityPlanner/World.h"

#include <cassert>
#include <optional>
#include <queue>
#include <set>
#include <unordered_map>

namespace CityPlanner {

namespace {

// Canonical side ordering: clockwise from top (y-down screen space).
// Side X is always opposite to side (X+3)%6.
//   0 = up        {  0, -1 }
//   1 = upper-right {  1, -1 }
//   2 = lower-right {  1,  0 }
//   3 = down      {  0,  1 }
//   4 = lower-left { -1,  1 }
//   5 = upper-left { -1,  0 }
static constexpr std::array<Types::hex_coord_t, 6> k_side_offsets = {{
    { 0, -1},
    { 1, -1},
    { 1,  0},
    { 0,  1},
    {-1,  1},
    {-1,  0},
}};

// ---------------------------------------------------------------------------
// BFS pathfinding
// ---------------------------------------------------------------------------

struct path_state_t {
    Types::hex_coord_t hex;
    int                dir; ///< Exit side of the cursor at this position.
    bool operator==(const path_state_t& o) const { return hex == o.hex && dir == o.dir; }
};

struct path_state_hash_t {
    size_t operator()(const path_state_t& s) const noexcept {
        return Types::HexHash{}(s.hex) ^ (std::hash<int>()(s.dir) << 20);
    }
};

/**
 * @brief BFS over (hex, direction) cursor states to find the shortest path
 *        that positions the cursor one step before @p to.
 *
 * The returned path does NOT include the final landing segment onto @p to;
 * that step is applied by the caller so it can choose the exit at @p to freely.
 *
 * @param from      Starting cursor hex.
 * @param from_dir  Starting cursor exit direction.
 * @param to        Target hex to approach.
 * @param to_entry  If set, the approach must produce this entry side at @p to.
 * @param world     Used to test that every intermediate tile is in-bounds.
 * @return Ordered segments forming the approach path, or nullopt if unreachable.
 */
static std::optional<RailNetwork::path_t> FindPath(
    Types::hex_coord_t        from,
    int                       from_dir,
    Types::hex_coord_t        to,
    std::optional<int>        to_entry,
    const CityPlanner::World& world,
    Track::track_type_t       type)
{
    // Goal: cursor at state S where S.hex + offset[S.dir] == to
    //       and (if to_entry set) (S.dir + 3) % 6 == *to_entry.
    auto is_goal = [&](const path_state_t& s) {
        const auto& off = k_side_offsets[s.dir];
        Types::hex_coord_t next = {s.hex.q + off.q, s.hex.r + off.r};
        if (!(next == to)) return false;
        return !to_entry.has_value() || (s.dir + 3) % 6 == *to_entry;
    };

    path_state_t start{from, from_dir};
    if (is_goal(start)) return RailNetwork::path_t{};

    const bool blocks_rivers = !Track::GetConfig(type).can_cross_rivers;

    // came_from maps each visited state to its predecessor.
    // The start sentinel uses predecessor == start itself.
    std::unordered_map<path_state_t, path_state_t, path_state_hash_t> came_from;
    std::queue<path_state_t> q;
    came_from[start] = start;
    q.push(start);

    std::optional<path_state_t> goal_state;

    while (!q.empty() && !goal_state) {
        path_state_t cur = q.front();
        q.pop();

        for (int off : {3, 2, 4}) { // Straight, CurveRight, CurveLeft
            const auto& d      = k_side_offsets[cur.dir];
            Types::hex_coord_t next_hex = {cur.hex.q + d.q, cur.hex.r + d.r};
            if (!world.InBounds(next_hex)) continue;
            if (blocks_rivers) {
                const auto* tile = world.GetTileConst(next_hex);
                if (tile && tile->is_river) continue;
            }

            int new_dir;
            if      (off == 3) new_dir = cur.dir;
            else if (off == 2) new_dir = (cur.dir + 5) % 6; // CurveRight turns clockwise
            else               new_dir = (cur.dir + 1) % 6; // CurveLeft turns counter-clockwise

            path_state_t next{next_hex, new_dir};
            if (came_from.count(next)) continue;
            came_from[next] = cur;

            if (is_goal(next)) { goal_state = next; break; }
            q.push(next);
        }
    }

    if (!goal_state) return std::nullopt;

    // Reconstruct the ordered sequence of cursor states from start to goal.
    std::vector<path_state_t> states;
    path_state_t cur = *goal_state;
    while (!(cur == came_from[cur])) { // stop at the start sentinel
        states.push_back(cur);
        cur = came_from[cur];
    }
    std::reverse(states.begin(), states.end());

    // Convert consecutive state pairs into placed segments.
    // Moving from state Si (hex, dir) to Si+1 (next_hex, new_dir) places a
    // segment at next_hex with entry=(dir+3)%6 and exit=new_dir.
    RailNetwork::path_t path;
    path.reserve(states.size());
    path_state_t prev = start;
    for (const auto& s : states) {
        path.push_back({s.hex, (prev.dir + 3) % 6, s.dir, type});
        prev = s;
    }
    return path;
}

} // namespace

// ---------------------------------------------------------------------------
// Static helpers
// ---------------------------------------------------------------------------

bool RailNetworkBuilder::IsValidAngle(side_t entry, side_t exit) {
    if (entry < 0 || entry > 5 || exit < 0 || exit > 5) return false;
    int diff = ((exit - entry) % 6 + 6) % 6;
    return diff == 2 || diff == 3 || diff == 4;
}

// ---------------------------------------------------------------------------
// Cursor API
// ---------------------------------------------------------------------------

RailNetwork::segment_t
RailNetworkBuilder::Start(Types::hex_coord_t hex, side_t exit, Track::track_type_t track_type) const {
    return {hex, /*entry=*/-1, exit, track_type};
}

RailNetwork::segment_t RailNetworkBuilder::BuildStraight(RailNetwork::segment_t begin) {
    return Advance(begin, 3);
}

RailNetwork::segment_t RailNetworkBuilder::BuildCurveRight(RailNetwork::segment_t begin) {
    return Advance(begin, 2);
}

RailNetwork::segment_t RailNetworkBuilder::BuildCurveLeft(RailNetwork::segment_t begin) {
    return Advance(begin, 4);
}

// ---------------------------------------------------------------------------
// Low-level placement
// ---------------------------------------------------------------------------

bool RailNetworkBuilder::AddSegment(Types::hex_coord_t  hex,
                                    side_t              entry,
                                    side_t              exit,
                                    Track::track_type_t track_type) {
    if (!IsValidAngle(entry, exit)) return false;

    auto &track = m_tracks[hex];

    // Reject exact duplicates before checking for junctions.
    for (const auto &existing : track.segments) {
        if (existing.entry == entry && existing.exit == exit) return false;
    }

    // Detect junctions: any shared side with an existing segment.
    for (const auto &existing : track.segments) {
        if (existing.entry == entry || existing.exit == exit ||
            existing.entry == exit  || existing.exit == entry) {
            track.has_junction = true;
        }
    }

    track.segments.push_back({hex, entry, exit, track_type});
    return true;
}

// ---------------------------------------------------------------------------
// Query
// ---------------------------------------------------------------------------

const std::vector<RailNetwork::segment_t> *
RailNetworkBuilder::SegmentsAt(Types::hex_coord_t hex) const {
    auto it = m_tracks.find(hex);
    if (it == m_tracks.end()) return nullptr;
    return &it->second.segments;
}

bool RailNetworkBuilder::HasTrack(Types::hex_coord_t hex) const {
    return m_tracks.count(hex) > 0;
}

// ---------------------------------------------------------------------------
// Build
// ---------------------------------------------------------------------------

RailNetwork RailNetworkBuilder::Build() const {
    RailNetwork network;

    // Pass 1: insert a node for every junction tile.
    for (const auto &[hex, track] : m_tracks) {
        if (track.has_junction) {
            network.AddNode(hex);
        }
    }

    // Pass 2: walk from each junction node along every exit to find the next
    // junction, then add an edge. Deduplicate edges by (min, max) node pair.
    std::set<std::pair<RailNetwork::node_index_t, RailNetwork::node_index_t>> seen;

    for (const auto &[hex, track] : m_tracks) {
        RailNetwork::node_index_t node_a = network.NodeAt(hex);
        if (node_a == -1) continue;

        for (const auto &seg : track.segments) {
            Types::hex_coord_t current   = hex;
            side_t             exit_side = seg.exit;
            unsigned           length    = 0;

            while (true) {
                const auto &off = k_side_offsets[exit_side];
                Types::hex_coord_t next  = {current.q + off.q, current.r + off.r};
                side_t             entry = (exit_side + 3) % 6;

                const RailNetwork::segment_t *next_seg = FindSegmentWithEntry(next, entry);
                if (!next_seg) break; // dead end — no edge to add

                ++length;

                RailNetwork::node_index_t node_b = network.NodeAt(next);
                if (node_b != -1) {
                    auto key = std::make_pair(std::min(node_a, node_b),
                                              std::max(node_a, node_b));
                    if (seen.find(key) == seen.end()) {
                        network.AddEdge(node_a, node_b, length);
                        seen.insert(key);
                    }
                    break;
                }

                current   = next;
                exit_side = next_seg->exit;
            }
        }
    }

    // Pass 3: copy all placed segments as visual geometry into the network.
    // The renderer uses these records to draw per-tile curves without needing
    // to know about RailNetworkBuilder's internal representation.
    for (const auto &[hex, track] : m_tracks) {
        for (const auto &seg : track.segments) {
            network.AddSegment(seg);
        }
    }

    return network;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

RailNetwork::segment_t
RailNetworkBuilder::Advance(RailNetwork::segment_t begin, side_t exit_offset) {
    const auto &off   = k_side_offsets[begin.exit];
    Types::hex_coord_t next  = {begin.hex.q + off.q, begin.hex.r + off.r};
    side_t             entry = (begin.exit + 3) % 6;
    side_t             exit  = (entry + exit_offset) % 6;
    bool placed = AddSegment(next, entry, exit, begin.track_type);
    assert(placed && "Advance produced an invalid angle — logic error in exit_offset");
    return {next, entry, exit, begin.track_type};
}

const RailNetwork::segment_t *
RailNetworkBuilder::FindSegmentWithEntry(Types::hex_coord_t hex, side_t entry) const {
    auto it = m_tracks.find(hex);
    if (it == m_tracks.end()) return nullptr;
    for (const auto &seg : it->second.segments) {
        if (seg.entry == entry) return &seg;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// BuildTo helpers
// ---------------------------------------------------------------------------

RailNetwork::segment_t
RailNetworkBuilder::ApplyPath(RailNetwork::segment_t start, const RailNetwork::path_t& path) {
    RailNetwork::segment_t cur = start;
    for (const auto& seg : path) {
        bool placed = AddSegment(seg.hex, seg.entry, seg.exit, seg.track_type);
        assert(placed && "ApplyPath: segment duplicates existing track — path crosses itself");
        cur = seg;
    }
    return cur;
}

// ---------------------------------------------------------------------------
// Preview overloads (non-mutating)
// ---------------------------------------------------------------------------

namespace {
// Returns the exit direction of the last segment in path, or start.exit if empty.
inline int PreGoalExit(const RailNetwork::segment_t& start,
                       const RailNetwork::path_t&    path) noexcept {
    return path.empty() ? start.exit : path.back().exit;
}
} // namespace

std::optional<RailNetwork::path_t>
RailNetworkBuilder::Preview(RailNetwork::segment_t start, Types::hex_coord_t end, const World& world) const {
    auto path = FindPath(start.hex, start.exit, end, std::nullopt, world, start.track_type);
    if (!path) return std::nullopt;
    int d = PreGoalExit(start, *path);
    path->push_back({end, (d + 3) % 6, d, start.track_type}); // straight landing
    return path;
}

std::optional<RailNetwork::path_t>
RailNetworkBuilder::Preview(RailNetwork::segment_t start,
                            Types::hex_coord_t     end,
                            side_t                 input_side,
                            const World&           world) const {
    auto path = FindPath(start.hex, start.exit, end, input_side, world, start.track_type);
    if (!path) return std::nullopt;
    int d = PreGoalExit(start, *path);
    path->push_back({end, (d + 3) % 6, d, start.track_type}); // straight landing; entry == input_side
    return path;
}

std::optional<RailNetwork::path_t>
RailNetworkBuilder::Preview(RailNetwork::segment_t start, RailNetwork::segment_t end, const World& world) const {
    auto path = FindPath(start.hex, start.exit, end.hex, end.entry, world, start.track_type);
    if (!path) return std::nullopt;
    int landing_entry = (PreGoalExit(start, *path) + 3) % 6; // == end.entry
    for (int off : {2, 3, 4}) {
        if ((landing_entry + off) % 6 != end.exit) {
            path->push_back({end.hex, landing_entry, (landing_entry + off) % 6, start.track_type});
            return path;
        }
    }
    return std::nullopt; // unreachable
}

// ---------------------------------------------------------------------------
// BuildTo overloads
// ---------------------------------------------------------------------------

std::optional<RailNetwork::segment_t>
RailNetworkBuilder::BuildTo(RailNetwork::segment_t start, Types::hex_coord_t end, const World& world) {
    auto path = FindPath(start.hex, start.exit, end, std::nullopt, world, start.track_type);
    if (!path) return std::nullopt;
    RailNetwork::segment_t pre_goal = ApplyPath(start, *path);
    return Advance(pre_goal, 3); // straight landing; any approach direction is acceptable
}

std::optional<RailNetwork::segment_t>
RailNetworkBuilder::BuildTo(RailNetwork::segment_t start,
                            Types::hex_coord_t     end,
                            side_t                 input_side,
                            const World&           world) {
    auto path = FindPath(start.hex, start.exit, end, input_side, world, start.track_type);
    if (!path) return std::nullopt;
    RailNetwork::segment_t pre_goal = ApplyPath(start, *path);
    return Advance(pre_goal, 3); // entry at end == input_side by construction; straight exit
}

std::optional<RailNetwork::segment_t>
RailNetworkBuilder::BuildTo(RailNetwork::segment_t start, RailNetwork::segment_t end, const World& world) {
    auto path = FindPath(start.hex, start.exit, end.hex, end.entry, world, start.track_type);
    if (!path) return std::nullopt;
    RailNetwork::segment_t pre_goal = ApplyPath(start, *path);

    // Pick a landing offset whose exit won't duplicate the existing segment.
    // exit at end.hex for offset `off` == (end.entry + off) % 6.
    // Exactly one of {2, 3, 4} matches end.exit; skip that one.
    for (int off : {2, 3, 4}) {
        if ((end.entry + off) % 6 != end.exit) {
            return Advance(pre_goal, off);
        }
    }
    return std::nullopt; // unreachable: at most one offset is a duplicate
}

} // namespace CityPlanner
