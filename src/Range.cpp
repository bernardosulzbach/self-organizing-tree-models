#include "Range.hpp"

#include <stdexcept>

Range::Range(float minimum, float maximum) : minimum(minimum), maximum(maximum) {
}

Range Range::merge(Range other) const {
  const auto min = std::min(minimum, other.minimum);
  const auto max = std::max(maximum, other.maximum);
  return Range(min, max);
}

float Range::interpolate(U64 value, U64 resolution) const {
  if (value > resolution) {
    throw std::domain_error("Value can be at most resolution.");
  }
  const auto factor = static_cast<float>(value) / static_cast<float>(resolution);
  return minimum + factor * (maximum - minimum);
}

float Range::getAverage() const {
  return minimum + 0.5f * (maximum - minimum);
}

float Range::getLength() const {
  return maximum - minimum;
}
