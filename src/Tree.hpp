#pragma once

#include <memory>

#include "Environment.hpp"
#include "Metamer.hpp"
#include "Point.hpp"

class Tree {
public:
  std::unique_ptr<Metamer> root;

  Tree(Environment &environment, Point seedlingPosition);

  /**
   * Performs a growth iteration.
   *
   * 1. Calculate local environment of all tree buds.
   * 2. Determine the fate of each bud.
   * 3. Append new shoots (right now, shoots are only one metamer long).
   * 4. Shed branches (not implemented).
   * 5. Update internode width for all internodes.
   */
  void performGrowthIteration();
};
