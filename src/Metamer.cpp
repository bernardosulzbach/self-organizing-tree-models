#include "Metamer.hpp"

/**
 * Perturbates a vector in a random direction by the specified angle.
 */
static Vector randomPerturbation(Environment &environment, Vector originalVector, float angle) {
  Vector vx(1.0f, 0.0f, 0.0f);
  Vector vy(0.0f, 1.0f, 0.0f);
  Vector auxiliaryVector = vx;
  if (std::abs(originalVector.dot(vx)) > (1.0f - 1.0e-6f)) {
    auxiliaryVector = vy;
  }
  const auto crossVector = originalVector.cross(auxiliaryVector).normalize();
  const auto s = environment.splitMixGenerator.nextUniformInRange(0.0f, 1.0f);
  const auto r = environment.splitMixGenerator.nextUniformInRange(0.0f, 1.0f);
  const auto h = std::cos(angle);
  const auto phi = 2.0f * 4.0f * std::atan(1.0f) * s;
  const auto z = h + (1.0f - h) * r;
  const auto sinOfT = std::sqrt(1.0f - z * z);
  const auto x = std::cos(phi) * sinOfT;
  const auto y = std::sin(phi) * sinOfT;
  return auxiliaryVector.scale(x).add(crossVector.scale(y)).add(originalVector.scale(z));
}

Metamer::Metamer(Environment &environment, const Point &beginning, const Point &end)
    : beginning(beginning), end(end), axillaryDirection(randomPerturbation(environment, Vector(beginning, end), Environment::AxillaryPerturbationAngle)),
      axillaryId(environment.getNextBudId()), terminalId(environment.getNextBudId()) {
}

Point Metamer::getCenter() const {
  const auto centerX = beginning.x + 0.5f * (end.x - beginning.x);
  const auto centerY = beginning.y + 0.5f * (end.y - beginning.y);
  const auto centerZ = beginning.z + 0.5f * (end.z - beginning.z);
  return Point{centerX, centerY, centerZ};
}

float Metamer::getLength() const {
  return beginning.distance(end);
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

Range Metamer::getYRange() const {
  const auto min = std::min(beginning.y, end.y);
  const auto max = std::max(beginning.y, end.y);
  Range range(min, max);
  if (axillary) {
    range = range.merge(axillary->getYRange());
  }
  if (terminal) {
    range = range.merge(terminal->getYRange());
  }
  return range;
}