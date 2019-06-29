#include "MarkerSet.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "PointAverage.hpp"
#include "Random.hpp"

MarkerSet::MarkerSet(float sideLength, U64 resolution, U64 pointCount)
    : xRange(-0.5f * sideLength, +0.5 * sideLength), yRange(0.0f, sideLength),
      zRange(-0.5f * sideLength, +0.5 * sideLength), resolution(resolution) {
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
  const auto ranges = getRangesForSphere(center, radius);
  for (auto x = ranges.minX; x < ranges.maxX; x++) {
    for (auto y = ranges.minY; y < ranges.maxY; y++) {
      for (auto z = ranges.minZ; z < ranges.maxZ; z++) {
        auto &xyzVector = points[x][y][z];
        const auto predicate = [center, radius](Point &point) { return point.distance(center) < radius; };
        xyzVector.erase(std::remove_if(std::begin(xyzVector), std::end(xyzVector), predicate), std::end(xyzVector));
      }
    }
  }
}

SpaceAnalysis MarkerSet::analyze(Point origin, Vector direction, float theta, float r) const {
  Vector vectorSum{};
  auto foundMarker = false;
  const auto ranges = getRangesForSphere(origin, r);
  for (auto x = ranges.minX; x < ranges.maxX; x++) {
    for (auto y = ranges.minY; y < ranges.maxY; y++) {
      for (auto z = ranges.minZ; z < ranges.maxZ; z++) {
        for (const auto &point : points[x][y][z]) {
          // Is within the distance?
          if (point.distance(origin) < r) {
            // Is within angle?
            if (Vector(origin, point).angleBetween(direction) < theta) {
              foundMarker = true;
              vectorSum = vectorSum.add(Vector(origin, point).normalize());
            }
          }
        }
      }
    }
  }
  SpaceAnalysis spaceAnalysis{};
  if (foundMarker) {
    spaceAnalysis.q = 1.0f;
    spaceAnalysis.v = vectorSum.normalize();
  }
  return spaceAnalysis;
}

static Range getRange(Range range, float resolution, float x, float radius) {
  const auto minimum = range.minimum;
  const auto maximum = range.maximum;
  const auto step = (maximum - minimum) / resolution;
  const auto low = std::clamp(std::floor((x - radius - minimum) / step), 0.0f, resolution);
  const auto high = std::clamp(std::ceil((x + radius - minimum) / step), 0.0f, resolution);
  return Range(low, high);
}

MarkerSetRanges MarkerSet::getRangesForSphere(Point origin, float radius) const {
  MarkerSetRanges ranges;
  const auto xIndexRange = getRange(xRange, resolution, origin.x, radius);
  ranges.minX = static_cast<U64>(xIndexRange.minimum);
  ranges.maxX = static_cast<U64>(xIndexRange.maximum);
  const auto yIndexRange = getRange(yRange, resolution, origin.y, radius);
  ranges.minY = static_cast<U64>(yIndexRange.minimum);
  ranges.maxY = static_cast<U64>(yIndexRange.maximum);
  const auto zIndexRange = getRange(zRange, resolution, origin.z, radius);
  ranges.minZ = static_cast<U64>(zIndexRange.minimum);
  ranges.maxZ = static_cast<U64>(zIndexRange.maximum);
  return ranges;
}
