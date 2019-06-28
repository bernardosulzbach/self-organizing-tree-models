#pragma once

#include <memory>

#include "Environment.hpp"
#include "Metamer.hpp"
#include "Point.hpp"
#include "Types.hpp"

class Tree {
public:
  std::unique_ptr<Metamer> root;
  Environment &environment;

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

  U64 countMetamers() const;

private:
  std::unique_ptr<Metamer> attemptGrowth(Point origin, Vector direction);

  void performGrowthIteration(std::unique_ptr<Metamer> &metamer);

  void updateInternodeWidths(std::unique_ptr<Metamer> &metamer);
};
