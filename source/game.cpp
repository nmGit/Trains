#include "Game.h"

namespace Trains {

Game::Game() {}

void Game::Start() {

    m_canvas.Start();

    m_world_view.Start();

    while (m_canvas.Service() == Drafter::Canvas::ServiceResult::Continue) {
        m_world_view.Service();
        m_canvas.Service();
    }
}
} // namespace Trains
