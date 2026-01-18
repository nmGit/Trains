#include "Drafter/Shapes/Label.h"

#include "SDL3_ttf/SDL_ttf.h"

namespace Drafter {
Label::Label(Canvas &canvas, config_t config) {
    canvas.AddItem(*this);
}

void Label::Draw() {
    // Drawing logic for the label would go here

}
} // namespace Drafter