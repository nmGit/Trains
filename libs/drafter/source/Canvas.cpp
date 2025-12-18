#include "Drafter/Canvas.h"
namespace Drafter {

Canvas::Canvas() {}

void Canvas::CreateWindow() {
    // Create window
    SDL_SetAppMetadata("Example Renderer Clear", "1.0",
                       "com.example.renderer-clear");
    Log::Info(m_log_context, "Initializing SDL...");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Log::Error(m_log_context, "Couldn't initialize SDL: %s",
                   SDL_GetError());
        return;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", 640, 480,
                                     SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
        Log::Error(m_log_context, "Couldn't create window/renderer: %s",
                   SDL_GetError());
        return;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return;
}

void Canvas::Draw() {
    const double now = ((double)SDL_GetTicks()) /
                       1000.0; /* convert from milliseconds to seconds. */
    /* choose the color for the frame we will draw. The sine wave trick makes it
     * fade between colors smoothly. */
    SDL_SetRenderDrawColorFloat(
        renderer, 0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT); /* new color, full alpha. */

    /* clear the window to the draw color. */
    SDL_RenderClear(renderer);
    // 2. Set the color for drawing points (e.g., white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

   
    // Render all children
    for (auto &item : m_items) {
        item->Draw();
    }
    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);
}

void Canvas::Start() {
    CreateWindow();
}

SDL_Renderer *Canvas::GetRenderer() {
    return renderer;
}

Canvas::ServiceResult Canvas::Service() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            Log::Info(m_log_context, "SDL_EVENT_QUIT received");
            return ServiceResult::Quit;
        }
    }

    Draw();
    return ServiceResult::Continue;
}

void Canvas::AddItem(Shape &item) {
    m_items.push_back(&item);
}

} // namespace Drafter
