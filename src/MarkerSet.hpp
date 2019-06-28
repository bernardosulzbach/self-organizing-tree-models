#pragma once

#include <vector>

#include "MarkerSetRanges.hpp"
#include "Point.hpp"
#include "Range.hpp"
#include "SpaceAnalysis.hpp"
#include "Types.hpp"
#include "Vector.hpp"

class MarkerSet {
public:
  Range xRange;
  Range yRange;
  Range zRange;

  U64 resolution;

  std::vector<std::vector<std::vector<std::vector<Point>>>> points;

  MarkerSet(float sideLength, U64 resolution, U64 pointCount);

  void removeSphere(Point center, float radius);

  SpaceAnalysis analyze(Point origin, Vector direction, float theta, float r) const;

private:
  MarkerSetRanges getRangesForSphere(Point origin, float radius) const;
};
