#include "BoundingBox.hpp"

void BoundingBox::include(Point point) {
  xRange = xRange.merge(Range(point.x, point.x));
  yRange = yRange.merge(Range(point.y, point.y));
  zRange = zRange.merge(Range(point.z, point.z));
}

BoundingBox BoundingBox::merge(BoundingBox other) const {
  return BoundingBox{xRange.merge(other.xRange), yRange.merge(other.yRange), zRange.merge(other.zRange)};
}
