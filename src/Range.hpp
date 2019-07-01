#pragma once

#include "Types.hpp"

class Range {
public:
  float minimum{};
  float maximum{};

  Range(float minimum, float maximum);

  Range merge(Range other) const;

  float interpolate(U64 value, U64 resolution) const;

  float getAverage() const;

  float getLength() const;
};
