#pragma once

#include "CityPlanner/Rail/RailNetwork.h"
#include "CityPlanner/Rail/RailNetworkBuilder.h"
#include "CityPlanner/World.h"
#include "CityView.h"
#include "Drafter/Camera.h"
#include "Drafter/Drafter.h"
#include "Drafter/Shapes/HexGrid.h"
#include "TransportView.h"
#include "Types/Types.h"

#include <SDL3/SDL.h>
#include <memory>
#include <optional>
#include <vector>

namespace Trains {
/**
 * @brief The WorldView class is responsible for visualizing the world.
 *
 * It uses the Drafter::Canvas to render the current state of the
 * CityPlanner::World.  Pan and zoom are handled by a Drafter::Camera
 * attached to the canvas.
 */
class WorldView {
  public:
    /**
     * @brief Constructs a new WorldView object.
     *
     * @param world  Reference to the CityPlanner::World object to visualize.
     * @param canvas Reference to the Drafter::Canvas object to draw on.
     */
    WorldView(CityPlanner::World &world, Drafter::Canvas &canvas);

    /**
     * @brief Draws the current state of the world onto the canvas.
     */
    void Service();

    /**
     * @brief Starts the WorldView, initializing necessary components.
     */
    void Start();

    /**
     * @brief Attaches a rail network for rendering.
     *
     * @param network Reference to the RailNetwork to visualize.
     */
    void SetNetwork(const CityPlanner::RailNetwork &network);

    /**
     * @brief Optionally attaches an externally-owned builder for ghost preview.
     *
     * When set, hovering over a tile while a track cursor is active will call
     * Preview() and display the result as a ghost path.  Pass nullptr to
     * detach.
     *
     * @param builder Pointer to the builder, or nullptr to disable ghost
     * rendering.
     */
    void SetBuilder(const CityPlanner::RailNetworkBuilder *builder);

    /**
     * @brief Sets the starting cursor for ghost track preview.
     *
     * The ghost path is drawn from this cursor to the hovered tile each frame,
     * provided a builder has been attached via SetBuilder().  Pass std::nullopt
     * to stop preview rendering.
     *
     * @param cursor Last-placed segment cursor, or std::nullopt to clear.
     */
    void
    SetTrackCursor(std::optional<CityPlanner::RailNetwork::segment_t> cursor);

  protected:
    void SlotRegionAdded(CityPlanner::Region &region);
    void SlotCityAdded(CityPlanner::City &city);
    void SlotMouseMotion(const SDL_Event &event);

  private:
    /** @brief Renders the background hex grid. */
    void DrawHexGrid(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders terrain tiles (rivers, etc.) as filled hexagons. */
    void DrawTerrain(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders all city views. */
    void DrawCities(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders the hovered tile highlight. */
    void DrawHoveredTile(BLContext &ctx, const Drafter::draw_params_t &params);

    /** @brief Renders the transportation network (tracks, semaphores, trains).
     */
    void DrawTransport(const Drafter::draw_params_t &params);

    static constexpr float k_cell_radius = 10.f;

    Drafter::Canvas    &m_canvas;
    CityPlanner::World &m_world;
    Drafter::Camera     m_camera;

    std::optional<Drafter::HexGrid>        m_hex_grid;
    std::vector<std::unique_ptr<CityView>> m_city_views;
    std::optional<TransportView>           m_transport_view;
    std::optional<Types::hex_coord_t>      m_hovered_tile;

    const CityPlanner::RailNetworkBuilder             *m_builder = nullptr;
    std::optional<CityPlanner::RailNetwork::segment_t> m_track_cursor;

    LogContext m_log_context{"WorldView"};
};
} // namespace Trains
