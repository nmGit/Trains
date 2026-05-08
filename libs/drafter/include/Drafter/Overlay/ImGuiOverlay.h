#pragma once

#include "Drafter/Overlay/Overlay.h"

struct SDL_Renderer;

namespace Drafter {

/**
 * @brief Intermediate base class for overlays that use ImGui.
 *
 * Manages the shared ImGui context and SDL3/SDLRenderer3 backends via a
 * static reference count, so any number of ImGuiOverlay-derived instances
 * can coexist without double-initialising ImGui.
 *
 * Frame lifecycle (NewFrame / Render / RenderDrawData) is coordinated
 * across all registered ImGuiOverlay instances:
 *   - The first overlay whose RenderScreen() is called each frame opens
 *     the ImGui frame and becomes the frame owner.
 *   - Each overlay renders its own windows inside RenderImGuiContent().
 *   - The frame owner's FlushScreen() closes the frame and submits draw
 *     data to the SDL_Renderer.
 *
 * Subclasses should override RenderImGuiContent() rather than RenderScreen()
 * or FlushScreen().
 */
class ImGuiOverlay : public Overlay {
  public:
    /**
     * @brief Initialise ImGui (first instance only) and attach to canvas.
     *
     * @param canvas The canvas to attach to.
     */
    explicit ImGuiOverlay(Canvas &canvas);

    /**
     * @brief Detach from the canvas and shut down ImGui (last instance only).
     */
    ~ImGuiOverlay() override;

    /**
     * @brief Forward the event to ImGui and capture it if ImGui wants it.
     *
     * @param event The SDL event.
     * @return True if ImGui consumed the event.
     */
    bool Service(const SDL_Event &event) override;

    /**
     * @brief Open the ImGui frame (first caller per frame) then render content.
     *
     * Do not override this — override RenderImGuiContent() instead.
     */
    void RenderScreen() override final;

    /**
     * @brief Submit the ImGui frame to the renderer (frame owner only).
     *
     * Called by Canvas after all overlays have had RenderScreen() called.
     * Do not override this.
     */
    void FlushScreen() override final;

  protected:
    /**
     * @brief Override to render ImGui windows for this overlay.
     *
     * Called once per frame between ImGui::NewFrame() and ImGui::Render(),
     * with the ImGui context active.
     */
    virtual void RenderImGuiContent() = 0;

  private:
    static int           s_ref_count;    ///< Number of live ImGuiOverlay instances
    static bool          s_frame_begun;  ///< True between NewFrame and Render this tick
    static ImGuiOverlay *s_frame_owner;  ///< Instance responsible for flushing this frame
    static SDL_Renderer *s_renderer;     ///< Shared renderer (set on first init)
};

} // namespace Drafter
