#pragma once

namespace Drafter {
/**
 * @brief Represents a rectangle with width and height.
 */
struct rect_t {
    float w;
    float h;
};

struct point_t {
    float x;
    float y;
};

struct geometry_t {
    rect_t  size;
    point_t pos;
};
} // namespace Drafter