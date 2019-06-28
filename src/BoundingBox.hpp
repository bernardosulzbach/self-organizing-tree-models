#pragma once

#include <algorithm>
#include <limits>

class BoundingBox {
public:
  double minX = std::numeric_limits<double>::max();
  double minY = std::numeric_limits<double>::max();
  double maxX = std::numeric_limits<double>::min();
  double maxY = std::numeric_limits<double>::min();

  void include(double x, double y);

  double getXInNormalizedSpace(double x) const;

  double getYInNormalizedSpace(double y) const;

  void scale(double factor);
};
