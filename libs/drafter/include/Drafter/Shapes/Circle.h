#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

namespace Drafter {

class Circle : public Shape {
  public:
    struct config_t {
        point_t     position;
        float       radius    = 1.f;
        zoom_fade_t stroke_fade;  ///< Zoom range over which stroke fades out
        zoom_fade_t fill_fade;    ///< Zoom range over which fill fades out
    };

    explicit Circle(config_t config);
    ~Circle();
    void Draw(BLContext &ctx, draw_params_t params) override;

  private:
    config_t m_config;
};

} // namespace Drafter