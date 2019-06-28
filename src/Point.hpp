#pragma once

#include <string>

class Point {
public:
  float x{};
  float y{};
  float z{};

  Point() = default;

  Point(float x, float y, float z);

  Point translate(float dx, float dy, float dz);

  float distance(const Point &other) const;

  std::string toString() const;
};
