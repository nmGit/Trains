#include "CityPlanner/Rail/RailNetwork.h"

namespace CityPlanner {

RailNetwork::node_index_t RailNetwork::AddNode(Types::hex_coord_t position) {
    node_index_t idx = static_cast<node_index_t>(nodes_.size());
    nodes_.push_back({position, {}});
    spatial_index_[position] = idx;
    return idx;
}

void RailNetwork::AddEdge(node_index_t node_a, node_index_t node_b,
                          unsigned length) {
    auto edge_idx = static_cast<node_index_t>(edges_.size());
    edges_.push_back({node_a, node_b, length});
    nodes_[node_a].edge_indices.push_back(edge_idx);
    nodes_[node_b].edge_indices.push_back(edge_idx);
}

RailNetwork::node_index_t RailNetwork::NodeAt(Types::hex_coord_t position) const {
    auto it = spatial_index_.find(position);
    if (it == spatial_index_.end()) return -1;
    return it->second;
}

void RailNetwork::AddSegment(segment_t segment) {
    segments_.push_back(segment);
}

} // namespace CityPlanner
