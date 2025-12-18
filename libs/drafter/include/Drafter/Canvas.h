#pragma once
#include "Events/Event.h"
#include "Log/Log.h"
#include "Types.h"
#include "Shape.h"

#include <SDL3/SDL.h>

namespace Drafter {
/**
 * @brief The root of all ui objects.
 *
 * The canvas is the container that is drawn onto the screen, housing all other
 * ui elements.
 */
class Canvas : public Shape  {

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

    void AddItem(Shape &item);

    SDL_Renderer *GetRenderer();
  protected:
    void CreateWindow();
    void Draw();

  private:
    // We will use this renderer to draw into this window every frame.
    SDL_Window   *window   = NULL;
    SDL_Renderer *renderer = NULL;

    resize_event_t m_resizeEvent;

    LogContext m_log_context{"Drafter::Canvas"};

    std::vector<Shape *> m_items;
};
} // namespace Drafter