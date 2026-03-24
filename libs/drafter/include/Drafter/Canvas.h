#pragma once
#include "Events/Event.h"
#include "Log/Log.h"
#include "Shape.h"
#include "Types.h"

#include <SDL3/SDL.h>
#include <blend2d/blend2d.h>

namespace Drafter {

class Camera;

/**
 * @brief The root of all ui objects.
 *
 * The canvas is the container that is drawn onto the screen, housing all other
 * ui elements.
 */
class Canvas : public Shape {

  public:
    enum class ServiceResult { Continue, Quit };

    /**
     * @brief Constructor
     */
    Canvas();

    /**
     * @brief Initializes and starts the canvas.
     */
    void Start();

    /** @brief Event type emitted for each unhandled SDL event (input, etc.). */
    typedef Event<void, const SDL_Event &> sdl_event_t;

    /**
     * @brief Polls SDL events and begins a new frame.
     *
     * If a camera is attached, updates camera state and applies its transform
     * to the rendering context.  After this call, the context is ready for
     * world-space drawing.  Call Present() when drawing is complete.
     *
     * @return Quit if the window was closed, Continue otherwise.
     */
    ServiceResult Service();

    /**
     * @brief Ends the current frame and flips to screen.
     *
     * Restores the rendering context (undoing any camera transform) and
     * presents the frame.  Must be called once per frame after Service().
     */
    void Present();

    BLContext &GetRenderer();

    /** @brief Returns the event fired for each unhandled SDL event. */
    sdl_event_t &OnSDLEvent();

    /**
     * @brief Attach or detach a camera.
     *
     * When a camera is set, Canvas applies its transform before drawing and
     * calls Camera::Update() each frame.  Pass nullptr to detach.
     *
     * @param camera Pointer to the camera, or nullptr.
     */
    void SetCamera(Camera *camera);

    /**
     * @brief Returns the visible world-space rectangle.
     *
     * If a camera is attached, delegates to Camera::GetViewBounds().
     * Otherwise returns the full image rect in screen pixels.
     */
    bounds_t GetViewBounds();

    /**
     * @brief Returns view bounds and current zoom bundled as draw parameters.
     *
     * Convenience wrapper for passing to Shape::Draw() and CityView::Service().
     */
    draw_params_t GetDrawParams();

    // Canvas manages its own rendering lifecycle via Service(); it does not
    // participate in the immediate-mode Draw(ctx) call chain.
    void Draw(BLContext &, draw_params_t) override {}

  protected:
    void CreateWindow();
    void Draw();

  private:
    static constexpr unsigned int WINDOW_WIDTH  = 1000;
    static constexpr unsigned int WINDOW_HEIGHT = 500;

    // We will use this renderer to draw into this window every frame.
    SDL_Window   *window   = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture  *texture  = NULL;

    BLContextCreateInfo m_ctx_info; ///< Initialised in Canvas() with hardware thread count

    // Allocate a BLImage to draw on, which will be used as the canvas for
    // rendering.
    BLImage   img{WINDOW_WIDTH, WINDOW_HEIGHT, BL_FORMAT_PRGB32};
    BLContext ctx; ///< Started in Start() with m_ctx_info

    Camera        *m_camera = nullptr;
    resize_event_t m_resizeEvent;
    sdl_event_t    m_sdl_event;

    LogContext m_log_context{"Drafter::Canvas"};
};

} // namespace Drafter