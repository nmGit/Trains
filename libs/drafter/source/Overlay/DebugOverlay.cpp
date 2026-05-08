#include "Drafter/Overlay/DebugOverlay.h"

#include <imgui.h>

namespace Drafter {

DebugOverlay::DebugOverlay(Canvas &canvas) : ImGuiOverlay(canvas) {}

void DebugOverlay::RenderWorld(BLContext & /*ctx*/,
                               const draw_params_t & /*params*/) {
    // Reserved for world-space debug drawing (tile highlights, paths, etc.)
}

void DebugOverlay::RenderImGuiContent() {
    ImGui::SetNextWindowPos(ImVec2(10.f, 10.f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(200.f, 80.f), ImGuiCond_Once);
    ImGui::Begin("Debug");
    ImGui::Text("FPS:  %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("ms:   %.2f", 1000.f / ImGui::GetIO().Framerate);
    ImGui::End();
}

} // namespace Drafter
