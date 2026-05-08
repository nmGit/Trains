#include "Drafter/Overlay/Overlay.h"

#include "Drafter/Canvas.h"

namespace Drafter {

Overlay::~Overlay() {
    Detach();
}

void Overlay::Attach(Canvas &canvas) {
    if (m_canvas)
        Detach();
    m_canvas = &canvas;
    m_canvas->RegisterOverlay(this);
}

void Overlay::Detach() {
    if (!m_canvas)
        return;
    m_canvas->UnregisterOverlay(this);
    m_canvas = nullptr;
}

} // namespace Drafter
