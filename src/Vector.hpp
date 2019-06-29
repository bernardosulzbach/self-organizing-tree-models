#pragma once

#include "Point.hpp"

class Vector {
public:
  float x{};
  float y{};
  float z{};

  Vector() = default;

  Vector(Point a, Point b);

  Vector(float x, float y, float z);

  float evaluateNorm() const;

  Vector normalize() const;

  Vector scale(float factor) const;

  Vector add(Vector vector) const;

  float dot(Vector vector) const;

  float angleBetween(Vector vector) const;

  Vector cross(Vector other) const;
};
