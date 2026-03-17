#pragma once
#include "Events/Event.h"
#include "Log/Log.h"
#include "Shape.h"
#include "Types.h"

#include <SDL3/SDL.h>
#include <blend2d/blend2d.h>
namespace Drafter {
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

    ServiceResult Service();

    void          AddItem(Shape &item);
    BLContext &GetRenderer();

  protected:
    void CreateWindow();
    void Draw();

  private:
    static constexpr unsigned int WINDOW_WIDTH  = 480;
    static constexpr unsigned int WINDOW_HEIGHT = 480;

    // We will use this renderer to draw into this window every frame.
    SDL_Window   *window   = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture  *texture  = NULL;

    // Allocate a BLImage to draw on, which will be used as the canvas for
    // rendering.
    BLImage   img{WINDOW_WIDTH, WINDOW_HEIGHT, BL_FORMAT_PRGB32};
    BLContext ctx{img};

    resize_event_t m_resizeEvent;

    LogContext m_log_context{"Drafter::Canvas"};

    std::vector<Shape *> m_items;
};
} // namespace Drafter