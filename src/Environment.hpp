#pragma once

#include <cmath>

#include "MarkerSet.hpp"
#include "Types.hpp"

class Environment {
private:
  BudId nextBudId = 1;

public:
  // The fixed length of the metamers, in meters.
  // TODO: change this, as it is just wrong.
  // The internode length of the metamer is variable.
  // The things that are specified as N times this length have to be evaluated at runtime.
  static constexpr auto MetamerLength = 0.02f;

  static constexpr auto OccupancyRadius = 2.0f * MetamerLength;
  static constexpr auto PerceptionRadius = 4.0f * MetamerLength;

  // In radians. About 85 degrees.
  static constexpr auto PerceptionAngle = 1.5f;

  static constexpr auto BorchertHondaAlpha = 2.0f;
  static constexpr auto BorchertHondaLambda = 0.5f;

  MarkerSet markerSet;

  explicit Environment(const MarkerSet &markerSet);

  BudId getNextBudId();
};
