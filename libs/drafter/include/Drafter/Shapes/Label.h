#include "Drafter/Canvas.h"
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

namespace Drafter {

class Label : public Shape {
  public:
    struct config_t {
        point_t point;
    };

    Label(Canvas &canvas, config_t config);
    void Draw() override;

  protected:
  private:
};
} // namespace Drafter