#include "BoundingBox.hpp"

void BoundingBox::include(double x, double y) {
  minX = std::min(minX, x);
  minY = std::min(minY, y);
  maxX = std::max(maxX, x);
  maxY = std::max(maxY, y);
}

double getInInterval(double a, double b, double x) {
  return 2.0 * (x - a) / (b - a) - 1.0;
}

double BoundingBox::getXInNormalizedSpace(double x) const {
  return getInInterval(minX, maxX, x);
}

double BoundingBox::getYInNormalizedSpace(double y) const {
  return getInInterval(minY, maxY, y);
}

struct Range {
  double minimum;
  double maximum;

  Range(double minimum, double maximum) : minimum(minimum), maximum(maximum) {
  }

  void scale(double factor) {
    const auto length = maximum - minimum;
    const auto newLength = length * factor;
    const auto midPoint = minimum + length / 2.0;
    minimum = midPoint - newLength / 2.0;
    maximum = midPoint + newLength / 2.0;
  }
};

void BoundingBox::scale(double factor) {
  auto xRange = Range(minX, maxX);
  auto yRange = Range(minY, maxY);
  xRange.scale(factor);
  yRange.scale(factor);
  minX = xRange.minimum;
  minY = yRange.minimum;
  maxY = yRange.maximum;
  maxX = xRange.maximum;
}
