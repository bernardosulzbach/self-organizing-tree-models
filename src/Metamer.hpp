#pragma once

#include <memory>

#include "BoundingBox.hpp"
#include "Environment.hpp"
#include "Point.hpp"

class Metamer {
public:
  Point beginning{};
  Point end{};

  Vector axillaryDirection;

  bool hasLeaf = false;

  float width = 0.0f;

  // Null indicates a bud.
  std::unique_ptr<Metamer> axillary;
  BudId axillaryId{};

  // Null indicates a bud.
  std::unique_ptr<Metamer> terminal;
  BudId terminalId{};

  float light = 0.0f;

  float axillaryLight = 0.0f;
  float terminalLight = 0.0f;

  float growthResource = 0.0f;

  float axillaryGrowthResource = 0.0f;
  float terminalGrowthResource = 0.0f;

  Metamer(Environment &environment, const Point &beginning, const Point &end);

  Point getCenter() const;

  float getLength() const;

  U64 countMetamers() const;

  BoundingBox getBoundingBox() const;
};
