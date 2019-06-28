#include "Tree.hpp"

Tree::Tree(Environment &environment, Point seedlingPosition) {
  const auto end = seedlingPosition.translate(0.0f, Environment::MetamerLength, 0.0f);
  root = std::make_unique<Metamer>(seedlingPosition, end);
}

void Tree::performGrowthIteration() {
}
