#include "Drafter/Shapes/Circle.h"

#include <SDL3/SDL.h>

namespace Drafter {
Circle::Circle(Canvas &canvas, config_t config)
    : m_canvas(canvas), m_config(config) {
    canvas.AddItem(*this);
}

Circle::~Circle() {}

void Circle::Draw() {
    // Draw the circle using SDL
    SDL_SetRenderDrawColor(m_canvas.GetRenderer(), 255, 100, 0, 255); // Red color

    // Plot points around the circle
    std::vector<SDL_FPoint> points;
    int                     num_points = 30;
          // Plot points around the circle
    for (int i = 0; i < num_points; i++) {
        points.emplace_back(m_config.position.x +
                m_config.radius * std::sin(2 * SDL_PI_F * i / num_points),
            m_config.position.y +
                m_config.radius * std::cos(2 * SDL_PI_F * i / num_points));
    }

    // Connect the last segment to the first to complete the circle
    points.emplace_back(points.front());

    SDL_RenderLines(m_canvas.GetRenderer(), points.data(), points.size());
}
} // namespace Drafter