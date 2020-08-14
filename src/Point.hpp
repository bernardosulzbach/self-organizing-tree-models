#pragma once

#include "Types.hpp"

#include <string>

class Point {
public:
  F32 x{};
  F32 y{};
  F32 z{};

  Point() = default;

  Point(F32 x, F32 y, F32 z);

  Point translate(F32 dx, F32 dy, F32 dz);

  [[nodiscard]] F32 distance(const Point &other) const;

  [[nodiscard]] std::string toString() const;
};
