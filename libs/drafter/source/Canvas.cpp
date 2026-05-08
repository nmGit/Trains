#include "Drafter/Canvas.h"

#include "Drafter/Camera.h"
#include "Drafter/Overlay/Overlay.h"

#include <algorithm>
#include <thread>

namespace Drafter {

Canvas::Canvas() {
    m_ctx_info.thread_count = 4;
}

void Canvas::CreateWindow() {
    Resize({WINDOW_WIDTH, WINDOW_HEIGHT});

    SDL_SetAppMetadata("Example Renderer Clear", "1.0",
                       "com.example.renderer-clear");

    Log::Info(m_log_context, "Initializing SDL...");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        Log::Error(m_log_context, "Couldn't initialize SDL: %s",
                   SDL_GetError());
        return;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", WINDOW_WIDTH,
                                     WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE,
                                     &window, &renderer)) {
        Log::Error(m_log_context, "Couldn't create window/renderer: %s",
                   SDL_GetError());
        return;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH,
                                WINDOW_HEIGHT);
}

void Canvas::Draw() {
    uint32_t *pixels;
    int       pixel_pitch;

    if (SDL_LockTexture(texture, NULL, (void **)&pixels, &pixel_pitch)) {

        // Finalize all pending Blend2D rendering into img before reading pixels
        ctx.end();

        BLImageData img_data;
        img.get_data(&img_data);

        const int   row_bytes   = img_data.size.w * 4;
        const int   src_stride  = static_cast<int>(img_data.stride);
        const auto *src = static_cast<const uint8_t *>(img_data.pixel_data);
        auto       *dst = reinterpret_cast<uint8_t *>(pixels);
        for (int y = 0; y < img_data.size.h; ++y) {
            memcpy(dst + y * pixel_pitch, src + y * src_stride, row_bytes);
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);

        // Screen-space overlays (ImGui, HUD) draw on top of the Blend2D output.
        for (auto *overlay : m_overlays)
            overlay->RenderScreen();

        // Allow overlays to finalise batched rendering (e.g. ImGui frame flush).
        for (auto *overlay : m_overlays)
            overlay->FlushScreen();

        SDL_RenderPresent(renderer);

        // Restart the context with multithreaded rasterisation and fill
        // with an opaque background for the next frame.
        ctx.begin(img, m_ctx_info);
        ctx.set_fill_style(BLRgba32(0xFF272727));
        ctx.fill_all();
    } else {
        Log::Error(m_log_context, "Couldn't lock texture: %s", SDL_GetError());
    }
}

void Canvas::Start() {
    CreateWindow();

    // Begin the first frame's context with multithreaded rasterisation.
    ctx.begin(img, m_ctx_info);
    ctx.set_fill_style(BLRgba32(0xFF272727));
    ctx.fill_all();
}

BLContext &Canvas::GetRenderer() {
    return ctx;
}

Canvas::sdl_event_t &Canvas::OnSDLEvent() {
    return m_sdl_event;
}


void Canvas::SetCamera(Camera *camera) {
    m_camera = camera;
}

void Canvas::RegisterOverlay(Overlay *overlay) {
    m_overlays.push_back(overlay);
}

void Canvas::UnregisterOverlay(Overlay *overlay) {
    m_overlays.erase(std::remove(m_overlays.begin(), m_overlays.end(), overlay),
                     m_overlays.end());
}

bounds_t Canvas::GetViewBounds() {
    if (m_camera) {
        return m_camera->GetViewBounds(GetGeometry());
    }
    const auto &geo = GetGeometry();
    return {0.f, 0.f, geo.size.w, geo.size.h};
}

draw_params_t Canvas::GetDrawParams() {
    if (m_camera) {
        return {m_camera->GetViewBounds(GetGeometry()), m_camera->GetZoom()};
    }
    const auto &geo = GetGeometry();
    return {{0.f, 0.f, geo.size.w, geo.size.h}, 1.f};
}

Canvas::ServiceResult Canvas::Service() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            Log::Info(m_log_context, "SDL_EVENT_QUIT received");
            return ServiceResult::Quit;
        }

        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            const int w = event.window.data1;
            const int h = event.window.data2;

            ctx.end();

            img = BLImage(w, h, BL_FORMAT_PRGB32);

            SDL_DestroyTexture(texture);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING, w, h);

            Resize({static_cast<float>(w), static_cast<float>(h)});

            ctx.begin(img, m_ctx_info);
            ctx.set_fill_style(BLRgba32(0xFF272727));
            ctx.fill_all();
        }

        // Overlays see the event first and may capture it.
        bool captured = false;
        for (auto *overlay : m_overlays) {
            if (overlay->Service(event))
                captured = true;
        }
        if (!captured)
            m_sdl_event.Emit(event);
    }

    // Update camera and apply its transform so subsequent draws are in
    // world space.  Present() will restore this.
    if (m_camera) {
        m_camera->Update();
        ctx.save();
        m_camera->ApplyTransform(ctx, GetGeometry());
    }

    return ServiceResult::Continue;
}

void Canvas::Present() {
    if (m_camera) {
        // RenderWorld is called while the camera transform is still active.
        for (auto *overlay : m_overlays)
            overlay->RenderWorld(ctx, GetDrawParams());
        ctx.restore();
    }
    Draw();
}

} // namespace Drafter
