#pragma once

#include "CityPlanner/Rail/RailNetwork.h"
#include "Types/Types.h"

#include <optional>
#include <unordered_map>
#include <vector>

namespace CityPlanner {

class World;

/**
 * @brief Constructs a RailNetwork by progressively placing track segments on a hex grid.
 *
 * Tracks are laid using a cursor-based API. Call Start() to establish a cursor
 * at a hex tile facing a given exit side, then chain Build* calls to advance
 * the cursor tile-by-tile:
 *
 * @code
 *   auto cur = builder.Start({20, 10}, 0);
 *   cur = builder.BuildStraight(cur);
 *   cur = builder.BuildCurveRight(cur);
 *   cur = builder.BuildStraight(cur);
 * @endcode
 *
 * Each Build* call places one segment on the next tile and returns a new
 * cursor ready for the following call. Segments are defined by entry and exit
 * sides (0–5, each 60° apart). Only two track angles are valid:
 *   - Straight:     entry X → exit (X+3)%6   — 180°
 *   - Curve ±120°:  entry X → exit (X+2)%6   — +120°
 *                   entry X → exit (X+4)%6   — −120°
 *
 * When two segments on the same tile share a side, a junction is created and
 * a node is inserted into the network at that tile. When segments cross the
 * same tile without sharing any side, a crossing is recorded but no node is
 * created. Call Build() to produce the finished RailNetwork.
 */
class RailNetworkBuilder {
  public:
    /** @brief Integer side index in the range [0, 5], each 60° from the previous. */
    using side_t = int;

    /**
     * @brief Establishes a cursor at a tile without placing a segment.
     *
     * The returned cursor is the seed for the first Build* call. No track is
     * placed on @p hex by this call.  The cursor's entry field is set to −1 to
     * indicate no segment has been placed yet.  All subsequent Build* calls
     * inherit the track type from the cursor.
     *
     * @param hex        Axial coordinate of the starting tile.
     * @param exit       Side the first Build* call will depart from.
     * @param track_type Track type for all segments built from this cursor.
     * @return A cursor with entry = −1, positioned at @p hex facing @p exit.
     */
    RailNetwork::segment_t Start(Types::hex_coord_t  hex,
                                 side_t              exit,
                                 Track::track_type_t track_type = Track::track_type_t::kBasicTrack) const;

    /**
     * @brief Advances the cursor straight (180°) and places a segment.
     *
     * @param begin Cursor returned by Start() or a previous Build* call.
     * @return A new cursor positioned on the next tile, ready to continue.
     */
    RailNetwork::segment_t BuildStraight(RailNetwork::segment_t begin);

    /**
     * @brief Advances the cursor with a +120° right curve and places a segment.
     *
     * @param begin Cursor returned by Start() or a previous Build* call.
     * @return A new cursor positioned on the next tile, ready to continue.
     */
    RailNetwork::segment_t BuildCurveRight(RailNetwork::segment_t begin);

    /**
     * @brief Advances the cursor with a −120° left curve and places a segment.
     *
     * @param begin Cursor returned by Start() or a previous Build* call.
     * @return A new cursor positioned on the next tile, ready to continue.
     */
    RailNetwork::segment_t BuildCurveLeft(RailNetwork::segment_t begin);

    /**
     * @brief Places a track segment through a hex tile directly.
     *
     * Lower-level alternative to the Build* cursor API. If the tile already
     * has segments, the new segment is checked for crossings and junctions.
     *
     * @param hex        Axial coordinate of the tile.
     * @param entry      Side the track enters on (0–5).
     * @param exit       Side the track exits on (0–5).
     * @param track_type Track type for this segment.
     * @return True if the segment was placed; false if the angle is invalid.
     */
    bool AddSegment(Types::hex_coord_t  hex,
                    side_t              entry,
                    side_t              exit,
                    Track::track_type_t track_type = Track::track_type_t::kBasicTrack);

    /**
     * @brief Computes the track path from @p start to @p end without placing segments.
     *
     * Returns the same path that BuildTo would place, including the landing
     * segment, but leaves m_tracks unchanged.  Intended for ghost/preview rendering.
     *
     * @param start  Cursor returned by Start() or a previous Build* or BuildTo call.
     * @param end    Target axial coordinate.
     * @param world  Used to validate that every intermediate tile is in-bounds.
     * @return Path of segments, or std::nullopt if no route exists.
     */
    std::optional<RailNetwork::path_t> Preview(RailNetwork::segment_t start,
                                               Types::hex_coord_t     end,
                                               const World&           world) const;

    /**
     * @brief Computes the track path from @p start to @p end arriving on @p input_side,
     *        without placing segments.
     *
     * @param start       Cursor returned by Start() or a previous Build* or BuildTo call.
     * @param end         Target axial coordinate.
     * @param input_side  Side (0–5) the track must enter @p end from.
     * @param world       Used to validate that every intermediate tile is in-bounds.
     * @return Path of segments, or std::nullopt if no route exists.
     */
    std::optional<RailNetwork::path_t> Preview(RailNetwork::segment_t start,
                                               Types::hex_coord_t     end,
                                               side_t                 input_side,
                                               const World&           world) const;

    /**
     * @brief Computes the track path from @p start to @p end.hex sharing @p end's
     *        entry side, without placing segments.
     *
     * @param start  Cursor returned by Start() or a previous Build* or BuildTo call.
     * @param end    An existing segment to connect to.
     * @param world  Used to validate that every intermediate tile is in-bounds.
     * @return Path of segments, or std::nullopt if no route exists.
     */
    std::optional<RailNetwork::path_t> Preview(RailNetwork::segment_t start,
                                               RailNetwork::segment_t end,
                                               const World&           world) const;

    /**
     * @brief Routes track from @p start to the hex nearest to @p end, choosing
     *        whichever approach direction yields the fewest segments.
     *
     * Uses BFS over (hex, direction) states, respecting world bounds.
     *
     * @param start  Cursor returned by Start() or a previous Build* or BuildTo call.
     * @param end    Target axial coordinate.
     * @param world  Used to validate that every placed tile is in-bounds.
     * @return Cursor positioned on @p end, or std::nullopt if no path exists.
     */
    std::optional<RailNetwork::segment_t> BuildTo(RailNetwork::segment_t start,
                                                  Types::hex_coord_t     end,
                                                  const World&           world);

    /**
     * @brief Routes track from @p start to @p end, arriving on @p input_side.
     *
     * The final segment at @p end will have entry == @p input_side.
     *
     * @param start       Cursor returned by Start() or a previous Build* or BuildTo call.
     * @param end         Target axial coordinate.
     * @param input_side  Side (0–5) the track must enter @p end from.
     * @param world       Used to validate that every placed tile is in-bounds.
     * @return Cursor positioned on @p end, or std::nullopt if no path exists.
     */
    std::optional<RailNetwork::segment_t> BuildTo(RailNetwork::segment_t start,
                                                  Types::hex_coord_t     end,
                                                  side_t                 input_side,
                                                  const World&           world);

    /**
     * @brief Routes track from @p start to the tile occupied by @p end,
     *        creating a junction that shares @p end's entry side.
     *
     * The placed segment at @p end.hex will have the same entry side as @p end,
     * triggering junction creation there.  The exit side of the new segment is
     * chosen to avoid duplicating @p end exactly.
     *
     * @param start  Cursor returned by Start() or a previous Build* or BuildTo call.
     * @param end    An existing segment to connect to.
     * @param world  Used to validate that every placed tile is in-bounds.
     * @return Cursor positioned on @p end.hex, or std::nullopt if no path exists.
     */
    std::optional<RailNetwork::segment_t> BuildTo(RailNetwork::segment_t start,
                                                  RailNetwork::segment_t end,
                                                  const World&           world);

    /**
     * @brief Returns whether an entry/exit pair forms a valid track angle.
     *
     * Valid exits from entry side X: (X+3)%6 (straight), (X+2)%6 (+120°),
     * (X+4)%6 (−120°).
     *
     * @param entry Entry side (0–5).
     * @param exit  Exit side (0–5).
     * @return True if the angle is a straight or ±120° curve.
     */
    static bool IsValidAngle(side_t entry, side_t exit);

    /**
     * @brief Returns all segments currently placed on a given tile.
     *
     * @param hex Axial coordinate of the tile to query.
     * @return Pointer to the segment list, or nullptr if the tile is empty.
     */
    const std::vector<RailNetwork::segment_t> *SegmentsAt(Types::hex_coord_t hex) const;

    /**
     * @brief Returns whether the given tile has any track segments.
     *
     * @param hex Axial coordinate of the tile.
     */
    bool HasTrack(Types::hex_coord_t hex) const;

    /**
     * @brief Builds and returns the completed RailNetwork.
     *
     * Nodes are placed at every junction. Edges connect adjacent nodes along
     * continuous stretches of track.
     *
     * @return The finished RailNetwork.
     */
    RailNetwork Build() const;

  private:
    struct hex_track_t {
        std::vector<RailNetwork::segment_t> segments;
        bool                                has_junction = false;
    };

    /** @brief Shared step used by all Build* methods. Places a segment on the tile
     *         reached by following begin's exit, then returns the new cursor. */
    RailNetwork::segment_t Advance(RailNetwork::segment_t begin, side_t exit_offset);

    /** @brief Applies every segment in @p path via AddSegment, returning the
     *         final segment as the new cursor. */
    RailNetwork::segment_t ApplyPath(RailNetwork::segment_t        start,
                                     const RailNetwork::path_t&    path);

    /** @brief Returns the segment on @p hex whose entry matches @p entry, or nullptr. */
    const RailNetwork::segment_t *FindSegmentWithEntry(Types::hex_coord_t hex,
                                                       side_t             entry) const;

    std::unordered_map<Types::hex_coord_t, hex_track_t, Types::HexHash> m_tracks;
};

} // namespace CityPlanner
