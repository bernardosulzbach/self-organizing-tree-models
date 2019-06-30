#pragma once

#include <memory>

#include "Environment.hpp"
#include "Point.hpp"

class Metamer {
public:
  Point beginning{};
  Point end{};

  bool hasLeaf = false;

  float width = Environment::MetamerLength / 8.0f;

  // Null indicates a bud.
  std::unique_ptr<Metamer> axillary;
  BudId axillaryId{};

  // Null indicates a bud.
  std::unique_ptr<Metamer> terminal;
  BudId terminalId{};

  float light = 0.0f;

  Metamer(Environment &environment, const Point &beginning, const Point &end);

  Point getCenter() const;

  U64 countMetamers() const;
};
