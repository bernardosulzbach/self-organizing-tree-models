#include "Metamer.hpp"

Metamer::Metamer(Environment &environment, const Point &beginning, const Point &end)
    : beginning(beginning), end(end), axillaryId(environment.getNextBudId()), terminalId(environment.getNextBudId()) {
}

Point Metamer::getCenter() const {
  const auto centerX = beginning.x + 0.5f * (end.x - beginning.x);
  const auto centerY = beginning.y + 0.5f * (end.y - beginning.y);
  const auto centerZ = beginning.z + 0.5f * (end.z - beginning.z);
  return Point{centerX, centerY, centerZ};
}

U64 Metamer::countMetamers() const {
  U64 count = 1;
  if (axillary) {
    count += axillary->countMetamers();
  }
  if (terminal) {
    count += terminal->countMetamers();
  }
  return count;
}