#pragma once
#include "CityPlanner/Rail/Track.h"
#include "Types/Types.h"
#include "Types/Utils.h"
#include <unordered_map>
#include <vector>

namespace CityPlanner {

/**
 * @brief A graph representation of a rail network.
 *
 * Nodes are junctions, endpoints, and switches — any point where connectivity
 * changes. Edges are contiguous stretches of track between two nodes.
 * A spatial index maps hex coordinates to node indices for O(1) lookup.
 *
 * In addition to the routing graph, the network stores the per-tile visual
 * geometry as a flat list of @p segment_t records. Each segment describes
 * the entry and exit side of the track through one hex tile, which is
 * sufficient for the renderer to draw the correct straight or curved line
 * without depending on RailNetworkBuilder.
 */
class RailNetwork {
  public:
    using node_index_t = int;

    /** @brief Integer side index in the range [0, 5], each 60° from the previous. */
    using side_t = int;

    /**
     * @brief A single track segment occupying one hex tile, as visual geometry.
     *
     * Describes the path of track through a tile by its entry and exit sides.
     * The renderer uses the side indices to compute edge midpoints and draw
     * either a straight line (sides 180° apart) or a quadratic Bezier curve
     * (sides 120° apart).
     */
    struct segment_t {
        Types::hex_coord_t      hex;       ///< Axial coordinate of the tile.
        side_t                  entry;     ///< Side the track enters on (0–5).
        side_t                  exit;      ///< Side the track exits on (0–5).
        Track::track_type_t     track_type = Track::track_type_t::kBasicTrack;
    };

    struct Node {
        Types::hex_coord_t        position;
        std::vector<node_index_t> edge_indices; // edges touching this node
    };

    struct Edge {
        node_index_t node_a;
        node_index_t node_b;
        unsigned     length = 0;
    };

    /** @brief An ordered sequence of nodes traversed by a train route. */
    using route_t = std::vector<node_index_t>;

    /** @brief An ordered sequence of track segments forming a continuous path. */
    using path_t = std::vector<segment_t>;

    /** @brief Adds a node at the given hex coordinate and returns its index. */
    node_index_t AddNode(Types::hex_coord_t position);

    /**
     * @brief Adds an edge between two existing nodes.
     * @param node_a Index of the first node.
     * @param node_b Index of the second node.
     * @param length Track length in hex steps.
     */
    void AddEdge(node_index_t node_a, node_index_t node_b, unsigned length);

    /**
     * @brief Returns the node index at a hex coordinate, or -1 if none.
     * @param position Hex coordinate to query.
     */
    node_index_t NodeAt(Types::hex_coord_t position) const;

    /**
     * @brief Appends a visual segment record to the network.
     *
     * Called by RailNetworkBuilder::Build() to populate the geometry data.
     * Normal callers should use RailNetworkBuilder rather than this method
     * directly.
     *
     * @param segment The segment to record.
     */
    void AddSegment(segment_t segment);

    /** @brief Returns all nodes in the network. */
    const std::vector<Node> &GetNodes() const { return nodes_; }

    /** @brief Returns all edges in the network. */
    const std::vector<Edge> &GetEdges() const { return edges_; }

    /**
     * @brief Returns all visual track segments in the network.
     *
     * The returned list is a flat collection of every placed segment across
     * all hex tiles, in the order they were added during Build(). Sufficient
     * for the renderer to draw all track geometry without any further lookup.
     */
    const std::vector<segment_t> &GetSegments() const { return segments_; }

  private:
    std::vector<Node>      nodes_;
    std::vector<Edge>      edges_;
    std::vector<segment_t> segments_;
    std::unordered_map<Types::hex_coord_t, int, Types::HexHash> spatial_index_;
};

} // namespace CityPlanner
