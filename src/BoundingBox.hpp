#pragma once

#include <algorithm>
#include <limits>

#include "Point.hpp"
#include "Range.hpp"

class BoundingBox {
public:
  Range xRange{};
  Range yRange{};
  Range zRange{};

  BoundingBox() = default;

  void include(Point point);

  BoundingBox merge(BoundingBox other) const;
};
