#pragma once
#include "Drafter/Shape.h"
#include "Drafter/Types.h"

namespace Drafter {

class Circle : public Shape {
  public:
    struct config_t {
        point_t position;
        float   radius;
    };

    explicit Circle(config_t config);
    ~Circle();
    void Draw(BLContext &ctx) override;

  private:
    config_t m_config;
};

} // namespace Drafter