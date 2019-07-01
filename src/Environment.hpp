#pragma once

#include <cmath>

#include "MarkerSet.hpp"
#include "Random.hpp"
#include "Types.hpp"

class Environment {
private:
  BudId nextBudId = 1;

public:
  static constexpr auto Pi = 3.1415926535897932384626433832795f;

  // In meters.
  static constexpr auto MetamerBaseLength = 0.01f;

  static constexpr auto OccupancyRadiusFactor = 2.0f;
  static constexpr auto PerceptionRadiusFactor = 4.0f;

  static constexpr auto PerceptionAngle = Pi / 2.0f;

  static constexpr auto AxillaryPerturbationAngle = Pi / 18.0f;

  static constexpr auto BorchertHondaAlpha = 2.0f;
  static constexpr auto BorchertHondaLambda = 0.5f;

  static constexpr auto OptimalGrowthDirectionWeight = 0.2f;

  SplitMixGenerator splitMixGenerator;
  MarkerSet markerSet;

  Environment(const SplitMixGenerator &SplitMixGenerator, MarkerSet markerSet);

  BudId getNextBudId();
};
