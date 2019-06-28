#include "MarkerSet.hpp"

#include <algorithm>
#include <stdexcept>

#include "PointAverage.hpp"
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
  // TODO: optimize this to skip over vectors which cannot intersect with the sphere.
  for (auto &xVector : points) {
    for (auto &xyVector : xVector) {
      for (auto &xyzVector : xyVector) {
        const auto predicate = [center, radius](Point &point) { return point.distance(center) < radius; };
        xyzVector.erase(std::remove_if(std::begin(xyzVector), std::end(xyzVector), predicate), std::end(xyzVector));
      }
    }
  }
}

SpaceAnalysis MarkerSet::analyze(Point origin, Vector direction, float theta, float r) const {
  // TODO: optimize this to skip over vectors which cannot intersect with the sphere.
  PointAverage pointAverage;
  for (auto &xVector : points) {
    for (auto &xyVector : xVector) {
      for (auto &xyzVector : xyVector) {
        for (auto &point : xyzVector) {
          // Is within the distance?
          if (point.distance(origin) < r) {
            // Is within angle?
            if (Vector(origin, point).angleBetween(direction) < theta) {
              pointAverage.update(point);
            }
          }
        }
      }
    }
  }
  SpaceAnalysis spaceAnalysis;
  if (pointAverage.count != 0.0f) {
    spaceAnalysis.q = 1.0f;
    spaceAnalysis.v = Vector(origin, pointAverage.average).normalize();
  }
  return spaceAnalysis;
}
