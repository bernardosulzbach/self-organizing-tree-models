#include "BoundingBox.hpp"

#include <sstream>

BoundingBox::BoundingBox(const std::string &string) {
  std::stringstream stream(string);
  stream >> xRange.minimum >> xRange.maximum;
  stream >> yRange.minimum >> yRange.maximum;
  stream >> zRange.minimum >> zRange.maximum;
}

BoundingBox::BoundingBox(const Range &xRange, const Range &yRange, const Range &zRange) : xRange(xRange), yRange(yRange), zRange(zRange) {
}

void BoundingBox::include(Point point) {
  xRange = xRange.merge(Range(point.x, point.x));
  yRange = yRange.merge(Range(point.y, point.y));
  zRange = zRange.merge(Range(point.z, point.z));
}

BoundingBox BoundingBox::merge(BoundingBox other) const {
  return BoundingBox{xRange.merge(other.xRange), yRange.merge(other.yRange), zRange.merge(other.zRange)};
}

std::string BoundingBox::toString() const {
  std::stringstream stream;
  stream << xRange.minimum << ' ';
  stream << xRange.maximum << ' ';
  stream << yRange.minimum << ' ';
  stream << yRange.maximum << ' ';
  stream << zRange.minimum << ' ';
  stream << zRange.maximum;
  return stream.str();
}
