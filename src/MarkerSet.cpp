#include "MarkerSet.hpp"

#include <stdexcept>

MarkerSet::MarkerSet(float sideLength, U64 resolution, U64 pointCount)
    : xRange(-0.5f * sideLength, +0.5 * sideLength), yRange(0.0f, sideLength),
      zRange(-0.5f * sideLength, +0.5 * sideLength) {
  if (sideLength <= 0.0f) {
    throw std::domain_error("Side length cannot be <= 0.0f!");
  }
  if (resolution < 1) {
    throw std::domain_error("Resolution must be at least 1.");
  }
  const auto boxes = resolution * resolution * resolution;
  if (pointCount % boxes != 0) {
    throw std::domain_error("Point count should be evenly divisible between the boxes.");
  }
}

void MarkerSet::removeSphere(Point center, float radius) {
  throw std::logic_error("Not implemented.");
}

SpaceAnalysis MarkerSet::analyze(Point origin, Vector direction, float radius, float length) const {
  throw std::logic_error("Not implemented.");
}
