#include "MarkerSet.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "PointAverage.hpp"
#include "Random.hpp"

MarkerSet::MarkerSet(SplitMixGenerator &splitMixGenerator, float sideLength, U64 resolution, U64 pointCount)
    : xRange(-0.5f * sideLength, +0.5 * sideLength), yRange(0.0f, sideLength), zRange(-0.5f * sideLength, +0.5 * sideLength), resolution(resolution) {
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
  for (U64 x = 0; x < resolution; x++) {
    markers.emplace_back();
    const auto xRangeMin = xRange.interpolate(x, resolution);
    const auto xRangeMax = xRange.interpolate(x + 1, resolution);
    for (U64 y = 0; y < resolution; y++) {
      markers[x].emplace_back();
      const auto yRangeMin = yRange.interpolate(y, resolution);
      const auto yRangeMax = yRange.interpolate(y + 1, resolution);
      for (U64 z = 0; z < resolution; z++) {
        markers[x][y].emplace_back();
        const auto zRangeMin = zRange.interpolate(z, resolution);
        const auto zRangeMax = zRange.interpolate(z + 1, resolution);
        for (U64 i = 0; i < pointsPerBox; i++) {
          markers[x][y][z].emplace_back();
          markers[x][y][z][i].position.x = splitMixGenerator.nextUniformInRange(xRangeMin, xRangeMax);
          markers[x][y][z][i].position.y = splitMixGenerator.nextUniformInRange(yRangeMin, yRangeMax);
          markers[x][y][z][i].position.z = splitMixGenerator.nextUniformInRange(zRangeMin, zRangeMax);
        }
      }
    }
  }
}

void MarkerSet::resetAllocations() {
  for (auto &xVector : markers) {
    for (auto &xyVector : xVector) {
      for (auto &xyzVector : xyVector) {
        for (auto &marker : xyzVector) {
          marker.resetAllocation();
        }
      }
    }
  }
}

void MarkerSet::updateAllocatedInCone(BudId budId, Point origin, Vector direction, float theta, float r) {
  const auto ranges = getRangesForSphere(origin, r);
  for (auto x = ranges.minX; x < ranges.maxX; x++) {
    for (auto y = ranges.minY; y < ranges.maxY; y++) {
      for (auto z = ranges.minZ; z < ranges.maxZ; z++) {
        for (auto &marker : markers[x][y][z]) {
          const auto point = marker.position;
          const auto distanceFromBud = point.distance(origin);
          // Is within the distance?
          if (distanceFromBud < r) {
            // Is less than the current allocated distance?
            if (distanceFromBud < marker.distanceToAllocated) {
              // Is within angle?
              if (Vector(origin, point).angleBetween(direction) < theta) {
                marker.allocationId = budId;
              }
            }
          }
        }
      }
    }
  }
}

SpaceAnalysis MarkerSet::getAllocatedInCone(BudId budId, Point origin, Vector direction, float theta, float r) const {
  Vector sumOfNormalizedVectors{};
  auto foundMarker = false;
  const auto ranges = getRangesForSphere(origin, r);
  for (auto x = ranges.minX; x < ranges.maxX; x++) {
    for (auto y = ranges.minY; y < ranges.maxY; y++) {
      for (auto z = ranges.minZ; z < ranges.maxZ; z++) {
        for (const auto &marker : markers[x][y][z]) {
          if (marker.allocationId != budId) {
            continue;
          }
          const auto point = marker.position;
          // Is within the distance?
          if (point.distance(origin) < r) {
            // Is within angle?
            if (Vector(origin, point).angleBetween(direction) < theta) {
              foundMarker = true;
              sumOfNormalizedVectors = sumOfNormalizedVectors.add(Vector(origin, point).normalize());
            }
          }
        }
      }
    }
  }
  SpaceAnalysis spaceAnalysis{};
  if (foundMarker) {
    spaceAnalysis.q = 1.0f;
    spaceAnalysis.v = sumOfNormalizedVectors.normalize();
  }
  return spaceAnalysis;
}

void MarkerSet::removeMarkersInSphere(Point center, float radius) {
  const auto ranges = getRangesForSphere(center, radius);
  for (auto x = ranges.minX; x < ranges.maxX; x++) {
    for (auto y = ranges.minY; y < ranges.maxY; y++) {
      for (auto z = ranges.minZ; z < ranges.maxZ; z++) {
        auto &xyzVector = markers[x][y][z];
        const auto predicate = [center, radius](Marker &marker) { return marker.position.distance(center) < radius; };
        xyzVector.erase(std::remove_if(std::begin(xyzVector), std::end(xyzVector), predicate), std::end(xyzVector));
      }
    }
  }
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
