#include "Tree.hpp"

Tree::Tree(Environment &environment, Point seedlingPosition) {
  const auto end = seedlingPosition.translate(0.0f, Environment::MetamerLength, 0.0f);
  root = std::make_unique<Metamer>(seedlingPosition, end);
}

/**
 * Performs a growth iteration.
 *
 * 1. Calculate local environment of all tree buds.
 * 2. Determine the fate of each bud.
 * 3. Append new shoots (right now, shoots are only one metamer long).
 * 4. Shed branches (not implemented).
 * 5. Update internode width for all internodes.
 */
void Tree::performGrowthIteration() {
}
