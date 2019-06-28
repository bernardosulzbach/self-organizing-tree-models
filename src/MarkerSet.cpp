#include "MarkerSet.hpp"

#include <stdexcept>

#include "Random.hpp"

MarkerSet::MarkerSet(float sideLength, U64 resolution, U64 pointCount)
    : xRange(-0.5f * sideLength, +0.5 * sideLength), yRange(0.0f, sideLength),
      zRange(-0.5f * sideLength, +0.5 * sideLength) {
  if (sideLength <= 0.0f) {
    throw std::domain_error("Side length cannot be <= 0.0f.");
  }
  if (resolution < 1) {
    throw std::domain_error("Resolution must be at least 1.");
  }
  const auto boxes = resolution * resolution * resolution;
  if (pointCount % boxes != 0) {
    throw std::domain_error("Point count should be evenly divisible between the boxes.");
  }
  const auto pointsPerBox = pointCount / boxes;
  SplitMixGenerator splitMixGenerator;
  for (U64 x = 0; x < resolution; x++) {
    points.emplace_back();
    const auto xRangeMin = xRange.interpolate(x, resolution);
    const auto xRangeMax = xRange.interpolate(x + 1, resolution);
    for (U64 y = 0; y < resolution; y++) {
      points[x].emplace_back();
      const auto yRangeMin = yRange.interpolate(y, resolution);
      const auto yRangeMax = yRange.interpolate(y + 1, resolution);
      for (U64 z = 0; z < resolution; z++) {
        points[x][y].emplace_back();
        const auto zRangeMin = zRange.interpolate(z, resolution);
        const auto zRangeMax = zRange.interpolate(z + 1, resolution);
        for (U64 i = 0; i < pointsPerBox; i++) {
          points[x][y][z].emplace_back();
          points[x][y][z][i].x = splitMixGenerator.nextUniformInRange(xRangeMin, xRangeMax);
          points[x][y][z][i].y = splitMixGenerator.nextUniformInRange(yRangeMin, yRangeMax);
          points[x][y][z][i].z = splitMixGenerator.nextUniformInRange(zRangeMin, zRangeMax);
        }
      }
    }
  }
}

void MarkerSet::removeSphere(Point center, float radius) {
  throw std::logic_error("Not implemented.");
}

SpaceAnalysis MarkerSet::analyze(Point origin, Vector direction, float radius, float length) const {
  throw std::logic_error("Not implemented.");
}
