#pragma once

#include "Drafter/Camera.h"
#include "Drafter/Canvas.h"
#include "Drafter/Overlay/Overlay.h"
#include "Drafter/Utils/Font.h"

#include <functional>
#include <string>
#include <vector>

namespace Trains {

/**
 * @brief Screen-space radial context menu drawn with Blend2D.
 *
 * Right-click opens the menu at the cursor.  Moving the mouse into a slice
 * highlights it.  Releasing the right mouse button selects the highlighted
 * slice (or cancels if the cursor is not over a slice).  Escape also cancels.
 *
 * Requires at least 3 options so each slice subtends less than 180°.
 */
class PieMenu : public Drafter::Overlay {
  public:
    /**
     * @brief Construct and attach to a canvas.
     *
     * @param camera Interface camera (reserved for future world-coord queries).
     * @param canvas The canvas to attach to.
     */
    PieMenu(Drafter::Camera &camera, Drafter::Canvas &canvas);
    ~PieMenu() override;

    /**
     * @brief Add an option to the pie menu.
     *
     * @param label     Text displayed in the slice.
     * @param on_select Callback fired when the option is chosen.
     */
    void AddOption(std::string label, std::function<void()> on_select);

    /**
     * @brief Load the font used to label slices.
     *
     * If not called, or if the font file cannot be opened, slices render
     * without text labels.
     *
     * @param path Filesystem path to a TrueType or OpenType font file.
     * @param size Pixel size.
     */
    void SetFont(const char *path, float size);

    /**
     * @brief Handle SDL input: detect right-click open/close, track hover.
     *
     * @param event The SDL event.
     * @return True to prevent the event from reaching the game.
     */
    bool Service(const SDL_Event &event) override;

    /**
     * @brief Draw the pie menu in screen space using Blend2D.
     *
     * Temporarily resets the BLContext transform so geometry is specified in
     * screen pixels regardless of the active camera.
     *
     * @param ctx    The active Blend2D context (camera transform applied by canvas).
     * @param params Current view bounds and zoom (unused by screen-space drawing).
     */
    void RenderWorld(BLContext &ctx, const Drafter::draw_params_t &params) override;

  private:
    struct Option {
        std::string           label;
        std::function<void()> on_select;
    };

    void Close();
    void UpdateHover();

    Drafter::Camera    &m_camera;
    Drafter::Font       m_font;
    std::vector<Option> m_options;
    bool                m_open     = false;
    float               m_center_x = 0.f;
    float               m_center_y = 0.f;
    float               m_mouse_x  = 0.f;
    float               m_mouse_y  = 0.f;
    int                 m_hovered  = -1;
};

} // namespace Trains
