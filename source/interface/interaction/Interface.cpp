#include "Interface.h"

namespace Trains {

Interface::Interface(Drafter::Canvas &canvas) : m_canvas(canvas) {}

Interface::~Interface() = default;

void Interface::Start() {}

} // namespace Trains
