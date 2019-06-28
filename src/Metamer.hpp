#pragma once

#include <memory>

#include "Point.hpp"

class Metamer {
public:
  Point beginning{};
  Point end{};
  bool hasLeaf = false;

  // Null indicates a bud.
  std::unique_ptr<Metamer> axillary;

  // Null indicates a bud.
  std::unique_ptr<Metamer> terminal;

  Metamer(const Point &beginning, const Point &end);
};
