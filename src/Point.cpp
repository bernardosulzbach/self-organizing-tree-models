#include "Point.hpp"

Point::Point(float x, float y, float z) : x(x), y(y), z(z) {
}

Point Point::translate(float dx, float dy, float dz) {
  return Point(x + dx, y + dy, z + dz);
}
