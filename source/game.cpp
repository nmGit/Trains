#include "Game.h"

Trains::Game::Game() {}

void Trains::Game::Start() {

    m_canvas.Start();

    m_world_view.Start();

    while (m_canvas.Service() == Drafter::Canvas::ServiceResult::Continue) {
        m_world_view.Service();
        m_canvas.Service();
    }
}
