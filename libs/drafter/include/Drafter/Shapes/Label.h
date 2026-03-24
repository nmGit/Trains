#include "Drafter/Canvas.h"
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

namespace Drafter {

class Label : public Shape {
  public:
    struct config_t {
        point_t     point;
        zoom_fade_t fill_fade;  ///< Zoom range over which text fades out
    };

    Label(Canvas &canvas, config_t config);
    void Draw(BLContext &ctx, draw_params_t params) override;

  protected:
  private:
};
} // namespace Drafter