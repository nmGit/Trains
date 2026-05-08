#include "Drafter/Overlay/ImGuiOverlay.h"

#include "Drafter/Canvas.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace Drafter {

int           ImGuiOverlay::s_ref_count   = 0;
bool          ImGuiOverlay::s_frame_begun = false;
ImGuiOverlay *ImGuiOverlay::s_frame_owner = nullptr;
SDL_Renderer *ImGuiOverlay::s_renderer    = nullptr;

ImGuiOverlay::ImGuiOverlay(Canvas &canvas) {
    if (s_ref_count++ == 0) {
        s_renderer = canvas.GetSDLRenderer();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(canvas.GetSDLWindow(), s_renderer);
        ImGui_ImplSDLRenderer3_Init(s_renderer);
    }
    Attach(canvas);
}

ImGuiOverlay::~ImGuiOverlay() {
    // ~Overlay() calls Detach()
    if (--s_ref_count == 0) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        s_renderer = nullptr;
    }
}

bool ImGuiOverlay::Service(const SDL_Event &event) {
    ImGui_ImplSDL3_ProcessEvent(&event);

    const ImGuiIO &io = ImGui::GetIO();
    switch (event.type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_WHEEL:
        return io.WantCaptureMouse;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
    case SDL_EVENT_TEXT_INPUT:
        return io.WantCaptureKeyboard;
    default:
        return false;
    }
}

void ImGuiOverlay::RenderScreen() {
    if (!s_frame_begun) {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        s_frame_begun = true;
        s_frame_owner = this;
    }
    RenderImGuiContent();
}

void ImGuiOverlay::FlushScreen() {
    if (s_frame_owner != this)
        return;
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), s_renderer);
    s_frame_begun = false;
    s_frame_owner = nullptr;
}

} // namespace Drafter
