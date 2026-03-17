#include "Drafter/Canvas.h"
namespace Drafter {

Canvas::Canvas() {}

void Canvas::CreateWindow() {
    // Create window

#ifdef NAM
    ctx.clear_all();
    // Create a gradient fill for the background
    BLGradient linear(BLLinearGradientValues(0, 0, 0, 480));

    // Add some color stops
    linear.add_stop(0.0, BLRgba32(0xFFFF0000)); // Dark gray at the top
    linear.add_stop(1.0, BLRgba32(0xFF1000FF)); // Lighter gray at the bottom

    // set_fill_style can be used to set solid colors, gradients, or patterns
    ctx.set_fill_style(linear);

    ctx.fill_round_rect(40, 40, 400, 400, 45.5);
    ctx.end();
#endif // NAM

    // This is the Shape::Resize method, which emits the resize event.
    Resize({WINDOW_WIDTH, WINDOW_HEIGHT});

    // Set application metadata (optional, but good practice)
    SDL_SetAppMetadata("Example Renderer Clear", "1.0",
                       "com.example.renderer-clear");

    Log::Info(m_log_context, "Initializing SDL...");
#
    // Initialize SDL with video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Log::Error(m_log_context, "Couldn't initialize SDL: %s",
                   SDL_GetError());
        return;
    }

    // Create a resizable window and renderer
    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", WINDOW_WIDTH,
                                     WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE,
                                     &window, &renderer)) {
        Log::Error(m_log_context, "Couldn't create window/renderer: %s",
                   SDL_GetError());
        return;
    }

    // Set the logical presentation to letterbox mode with a fixed size of
    // 640x480
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // Create the streaming texture that will be used to render the output of
    // blend2d
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH,
                                WINDOW_HEIGHT);
    return;
}

void Canvas::Draw() {
    // Update the texture with the pixel data from the blend2d image
    // see
    // https://github.com/libsdl-org/SDL/blob/main/examples/renderer/07-streaming-textures/streaming-textures.c

    // The texture must be locked before we can write to it, and unlocked
    // afterwards. Use SDL_LockTexture to get a an array of raw pixels, but
    // SDL_LockTextureToSurface is a little more convenient because it returns
    // an SDL_Surface, which is easier to draw to.
    SDL_Surface *surface = NULL;
    SDL_FRect    dst_rect;
    uint32_t    *pixels;
    int          pixel_pitch;

    if (SDL_LockTexture(texture, NULL, (void **)&pixels, &pixel_pitch)) {

        // Finalize all pending Blend2D rendering into img before reading pixels
        ctx.end();

        BLImageData img_data;
        img.get_data(&img_data);

        memcpy(pixels, img_data.pixel_data,
               img_data.size.w * img_data.size.h * 4);

        SDL_UnlockTexture(texture);

        // SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);

        // Finally, present the renderer to the screen. This will show the
        // texture we just drew.
        SDL_RenderPresent(renderer);

        // Restart the context and clear for the next frame
        ctx.begin(img);
        ctx.set_fill_style(BLRgba32(0xFF1E1E2E));
        ctx.fill_all();
    } else {
        Log::Error(m_log_context, "Couldn't lock texture: %s", SDL_GetError());
        return;
    }
}

void Canvas::Start() {
    CreateWindow();
}

BLContext &Canvas::GetRenderer() {
    return ctx;
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
