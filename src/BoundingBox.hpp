#pragma once

#include <algorithm>
#include <limits>
#include <string>

#include "Point.hpp"
#include "Range.hpp"

class BoundingBox {
public:
  Range xRange{};
  Range yRange{};
  Range zRange{};

  BoundingBox() = default;

  explicit BoundingBox(const std::string &string);

  BoundingBox(const Range &xRange, const Range &yRange, const Range &zRange);

  void include(Point point);

  BoundingBox merge(BoundingBox other) const;

  std::string toString() const;
};
