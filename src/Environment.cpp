#include "Environment.hpp"

Environment::Environment(const MarkerSet &markerSet) : markerSet(markerSet) {
}

BudId Environment::getNextBudId() {
  const auto value = nextBudId;
  nextBudId++;
  return value;
}
