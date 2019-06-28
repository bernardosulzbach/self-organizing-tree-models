#include "Point.hpp"

#include <cmath>

Point::Point(float x, float y, float z) : x(x), y(y), z(z) {
}

Point Point::translate(float dx, float dy, float dz) {
  return Point(x + dx, y + dy, z + dz);
}

float Point::distance(const Point &other) const {
  const auto dx = other.x - x;
  const auto dy = other.y - y;
  const auto dz = other.z - z;
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

std::string Point::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}
