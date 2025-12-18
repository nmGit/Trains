#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Canvas.h"
#include "Drafter/Types.h"

namespace Drafter {
class Circle : public Shape {
  public:

      struct config_t {
          point_t position;
          float radius;
      };
    Circle(Canvas &canvas, config_t config);
    ~Circle() ;
    void Draw() override;


  protected:
  private:
    config_t m_config;
    Canvas  &m_canvas;
};
} // namespace Drafter