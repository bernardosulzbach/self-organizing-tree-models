#pragma once

#include <cmath>

class Environment {
public:
  // Everything is in meters, so this is 2 cm.
  static constexpr auto MetamerLength = 0.02;
  static constexpr auto OccupancyRadius = 2 * MetamerLength;
  static constexpr auto PerceptionRadius = 4 * MetamerLength;
  // Angle of 90 degrees.
  static constexpr auto PerceptionAngle = 2.0 * std::atan(1.0);
};
