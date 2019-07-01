#include <utility>

#include "Environment.hpp"

Environment::Environment(const SplitMixGenerator &splitMixGenerator, MarkerSet markerSet)
    : splitMixGenerator(splitMixGenerator), markerSet(std::move(markerSet)) {
}

BudId Environment::getNextBudId() {
  const auto value = nextBudId;
  nextBudId++;
  return value;
}
