#pragma once

#include "Drafter/Types.h"

#include <SDL3/SDL.h>

class BLContext;

namespace Drafter {

class Canvas;

/**
 * @brief Abstract base class for overlays rendered on top of the canvas.
 *
 * Overlays participate in the frame loop at two points:
 *   - RenderWorld() is called with the camera transform still active, allowing
 *     drawing in world space via Blend2D (e.g. debug tile highlights).
 *   - RenderScreen() is called after the Blend2D output is committed to the
 *     SDL_Renderer but before SDL_RenderPresent, allowing screen-space drawing
 *     (e.g. ImGui panels).
 *
 * Service() receives every SDL event before the canvas forwards it to the rest
 * of the application.  Returning true captures the event and prevents it from
 * reaching the game's input handlers.
 */
class Overlay {
  public:
    virtual ~Overlay();

    /**
     * @brief Process an SDL event.
     *
     * Called for every SDL event before the canvas emits it to other
     * subscribers.  Return true to capture the event and prevent it from
     * reaching the game.
     *
     * @param event The SDL event to process.
     * @return True if the event was consumed by this overlay.
     */
    virtual bool Service(const SDL_Event &event) { return false; }

    /**
     * @brief Draw world-space debug content via Blend2D.
     *
     * Called from Canvas::Present() while the camera transform is still
     * applied to @p ctx, so coordinates are in world space.
     *
     * @param ctx    The active Blend2D context.
     * @param params Current view bounds and zoom level.
     */
    virtual void RenderWorld(BLContext &ctx, const draw_params_t &params) {}

    /**
     * @brief Draw screen-space content via the SDL_Renderer.
     *
     * Called from Canvas::Draw() after the Blend2D texture has been
     * submitted to the SDL_Renderer but before SDL_RenderPresent.
     */
    virtual void RenderScreen() {}

    /**
     * @brief Finalise any batched screen-space rendering for this frame.
     *
     * Called by Canvas::Draw() once after all overlays have had
     * RenderScreen() called.  Use this to submit accumulated draw data
     * (e.g. ImGui::Render()) that must fire after all content is recorded.
     */
    virtual void FlushScreen() {}

    /**
     * @brief Register this overlay with a canvas.
     *
     * @param canvas The canvas to attach to.
     */
    void Attach(Canvas &canvas);

    /**
     * @brief Unregister this overlay from its canvas.
     */
    void Detach();

  protected:
    Canvas *m_canvas = nullptr;
};

} // namespace Drafter
