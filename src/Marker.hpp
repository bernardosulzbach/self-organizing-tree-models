#pragma once

#include <limits>

#include "Point.hpp"
#include "Types.hpp"

class Marker {
public:
  Point position{};
  U64 allocationId{};
  F32 distanceToAllocated = std::numeric_limits<float>::infinity();

  void resetAllocation();
};
