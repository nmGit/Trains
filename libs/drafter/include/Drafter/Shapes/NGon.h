#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

namespace Drafter {

class NGon : public Shape {
  public:
    struct config_t {
        point_t position;
        float   radius;
        size_t  sides;
    };

    explicit NGon(config_t config);
    ~NGon();
    void Draw(BLContext &ctx) override;

  private:
    config_t m_config;
};

} // namespace Drafter
