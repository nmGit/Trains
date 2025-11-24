#pragma once
#include "Drafter/Shape.h"

namespace Drafter {
class Circle : public Shape {
  public:
    Circle();
    ~Circle() ;
    void Draw() override;


  protected:
  private:
};
} // namespace Drafter