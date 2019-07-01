#pragma once

#include <vector>

#include "Marker.hpp"
#include "MarkerSetRanges.hpp"
#include "Point.hpp"
#include "Random.hpp"
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

  std::vector<std::vector<std::vector<std::vector<Marker>>>> markers;

  MarkerSet(SplitMixGenerator &splitMixGenerator, float sideLength, U64 resolution, U64 pointCount);

  void resetAllocations();

  void updateAllocatedInCone(BudId budId, Point origin, Vector direction, float theta, float r);

  SpaceAnalysis getAllocatedInCone(BudId budId, Point origin, Vector direction, float theta, float r) const;

  void removeMarkersInSphere(Point center, float radius);

private:
  MarkerSetRanges getRangesForSphere(Point origin, float radius) const;
};
