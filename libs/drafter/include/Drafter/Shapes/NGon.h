#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Canvas.h"
#include "Drafter/Types.h"

namespace Drafter {
class NGon : public Shape {
  public:
    struct config_t {
        point_t position;
        float   radius;
        size_t  sides;
    };
    NGon(Canvas &canvas, config_t config);
    ~NGon();
    void Draw() override;

  private:
    config_t  m_config;
    Canvas   &m_canvas;
};
} // namespace Drafter
