#pragma once

#include <memory>

#include "Environment.hpp"
#include "Metamer.hpp"
#include "Point.hpp"
#include "Types.hpp"

static const float TropismGrowthDirectionWeightAttenuation = 0.95f;

class Tree {
public:
  std::unique_ptr<Metamer> root;
  Environment &environment;

  float tropismGrowthDirectionWeight = 0.5f;

  Tree(Environment &environment, Point seedlingPosition);

  U64 countMetamers() const;

  Range getYRange() const;

  /**
   * Performs a growth iteration.
   *
   * 1. Calculate local environment of all tree buds.
   * 2. Determine the fate of each bud.
   * 3. Append new shoots.
   * 4. Shed branches (not implemented).
   * 5. Update internode width for all internodes.
   */
  void performGrowthIteration();

private:
  void allocateMarkers(std::unique_ptr<Metamer> &metamer);

  void propagateLightBasipetally(std::unique_ptr<Metamer> &metamer);

  static void propagateResourcesAcropetally(std::unique_ptr<Metamer> &metamer);

  void performGrowthIteration(std::unique_ptr<Metamer> &metamer);

  std::unique_ptr<Metamer> addNewShoot(BudId budId, float supportingMetamerLength, Point origin, Vector direction, float resource);

  static void updateInternodeWidths(std::unique_ptr<Metamer> &metamer);
};
