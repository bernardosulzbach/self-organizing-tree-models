#include "Range.hpp"

#include <stdexcept>

Range::Range(float minimum, float maximum) : minimum(minimum), maximum(maximum) {
}

float Range::interpolate(U64 value, U64 resolution) const {
  if (value > resolution) {
    throw std::domain_error("Value can be at most resolution.");
  }
  const auto factor = static_cast<float>(value) / static_cast<float>(resolution);
  return minimum + factor * (maximum - minimum);
}
