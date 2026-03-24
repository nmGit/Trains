#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

namespace Drafter {

class NGon : public Shape {
  public:
    struct config_t {
        point_t     position;
        float       radius = 1.f;
        size_t      sides  = 3;
        zoom_fade_t stroke_fade;  ///< Zoom range over which stroke fades out
        zoom_fade_t fill_fade;    ///< Zoom range over which fill fades out
    };

    explicit NGon(config_t config);
    ~NGon();
    void Draw(BLContext &ctx, draw_params_t params) override;

  private:
    config_t m_config;
};

} // namespace Drafter
