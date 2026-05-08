#pragma once

#include "CityPlanner/Rail/RailNetwork.h"
#include "Drafter/Canvas.h"
#include "Drafter/Types.h"

#include <blend2d/blend2d.h>
#include <optional>

namespace Trains {

/**
 * @brief Visualizes a rail network on the world hex grid.
 *
 * Each frame, Service() renders the transportation network in layers:
 * tracks first, then ghost (preview) tracks, then semaphores, then trains.
 */
class TransportView {
  public:
    TransportView(const CityPlanner::RailNetwork &network,
                  Drafter::Canvas &canvas, float cell_radius);

    void Service(Drafter::draw_params_t params);

    /**
     * @brief Sets the ghost path to display as a preview overlay this frame.
     *
     * Replaces any previously set ghost path.  Call ClearGhostPath() to stop
     * showing the preview.
     *
     * @param path Segments returned by RailNetworkBuilder::Preview().
     */
    void SetGhostPath(CityPlanner::RailNetwork::path_t path);

    /** @brief Removes the ghost path; no preview is rendered until the next SetGhostPath(). */
    void ClearGhostPath();

  private:
    /** @brief Renders committed rail lines. */
    void DrawTracks(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders the ghost preview path with reduced opacity. */
    void DrawGhostTracks(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders semaphore signals at junctions and switches. */
    void DrawSemaphores(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders trains at their current positions on the network. */
    void DrawTrains(BLContext &ctx, const Drafter::draw_params_t &params);

    const CityPlanner::RailNetwork &m_network;
    Drafter::Canvas                &m_canvas;
    float                           m_cell_radius;

    std::optional<CityPlanner::RailNetwork::path_t> m_ghost_path;
};

} // namespace Trains
