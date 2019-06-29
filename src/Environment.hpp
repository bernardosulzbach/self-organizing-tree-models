#pragma once

#include <cmath>

#include "MarkerSet.hpp"

class Environment {
public:
  // Everything is in meters, so this is 2 cm.
  static constexpr auto MetamerLength = 0.02;
  static constexpr auto OccupancyRadius = 2 * MetamerLength;
  static constexpr auto PerceptionRadius = 4 * MetamerLength;
  // About 85 degrees.
  static constexpr auto PerceptionAngle = 1.5;

  MarkerSet markerSet;

  explicit Environment(const MarkerSet &markerSet);
};
