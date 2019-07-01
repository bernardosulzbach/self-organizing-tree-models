#pragma once

#include <limits>

#include "Types.hpp"

class Range {
public:
  float minimum = std::numeric_limits<float>::max();
  float maximum = std::numeric_limits<float>::min();

  Range() = default;
  Range(float minimum, float maximum);

  Range merge(Range other) const;

  float interpolate(U64 value, U64 resolution) const;

  float getAverage() const;

  float getLength() const;
};
