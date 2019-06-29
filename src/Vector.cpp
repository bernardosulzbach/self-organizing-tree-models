#include "Vector.hpp"

#include <cmath>

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {
}

Vector::Vector(Point a, Point b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {
}

float Vector::evaluateNorm() const {
  return std::sqrt(x * x + y * y + z * z);
}

Vector Vector::normalize() const {
  return scale(1.0f / evaluateNorm());
}

Vector Vector::scale(float factor) const {
  return {x * factor, y * factor, z * factor};
}

Vector Vector::add(Vector other) const {
  return Vector(x + other.x, y + other.y, z + other.z);
}

float Vector::dot(Vector other) const {
  return x * other.x + y * other.y + z * other.z;
}

float Vector::angleBetween(Vector other) const {
  return std::acos(dot(other) / evaluateNorm() / other.evaluateNorm());
}

Vector Vector::cross(Vector other) const {
  const auto cx = y * other.z - z * other.y;
  const auto cy = z * other.x - x * other.z;
  const auto cz = x * other.y - y * other.x;
  return Vector(cx, cy, cz);
}
