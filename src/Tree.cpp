#include "Tree.hpp"

#include <iostream>

Tree::Tree(Environment &environment, Point seedlingPosition) : environment(environment) {
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
  // 1.
  // 2.
  // 3.
  performGrowthIteration(root);
  // 5.
  updateInternodeWidths(root);
}

std::unique_ptr<Metamer> Tree::attemptGrowth(Point origin, Vector direction) {
  const auto theta = Environment::PerceptionAngle;
  const auto r = Environment::PerceptionRadius;
  const auto analysis = environment.markerSet.analyze(origin, direction, theta, r);
  if (analysis.q == 1.0f) {
    const auto shootV = analysis.v.scale(Environment::MetamerLength);
    const auto shootEnd = origin.translate(shootV.x, shootV.y, shootV.z);
    environment.markerSet.removeSphere(shootEnd, Environment::OccupancyRadius);
    return std::make_unique<Metamer>(origin, shootEnd);
  }
  return nullptr;
}

void Tree::performGrowthIteration(std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  if (!metamer->axillary) {
    // TODO: change this direction somehow.
    const auto direction = Vector(metamer->beginning, metamer->end);
    metamer->axillary = attemptGrowth(metamer->end, direction);
  } else {
    performGrowthIteration(metamer->axillary);
  }
  if (!metamer->terminal) {
    const auto direction = Vector(metamer->beginning, metamer->end);
    metamer->terminal = attemptGrowth(metamer->end, direction);
  } else {
    performGrowthIteration(metamer->terminal);
  }
}

void Tree::updateInternodeWidths(std::unique_ptr<Metamer> &metamer) {
}

U64 Tree::countMetamers() const {
  if (!root) {
    return 0;
  }
  return root->countMetamers();
}
