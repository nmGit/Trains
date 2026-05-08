#pragma once

#include "Drafter/Overlay/ImGuiOverlay.h"

namespace Drafter {

/**
 * @brief ImGui-based debug overlay showing FPS and frame time.
 *
 * Derives from ImGuiOverlay, which manages the shared ImGui context and
 * frame lifecycle.  Multiple ImGuiOverlay-derived instances can coexist
 * without conflict.
 */
class DebugOverlay : public ImGuiOverlay {
  public:
    /**
     * @brief Construct and attach to a canvas.
     *
     * @param canvas The canvas to attach to.
     */
    explicit DebugOverlay(Canvas &canvas);

    /**
     * @brief Draw world-space debug geometry via Blend2D.
     *
     * @param ctx    The active Blend2D context (camera transform applied).
     * @param params Current view bounds and zoom level.
     */
    void RenderWorld(BLContext &ctx, const draw_params_t &params) override;

  protected:
    /** @brief Render the FPS/frame-time ImGui window. */
    void RenderImGuiContent() override;
};

} // namespace Drafter
