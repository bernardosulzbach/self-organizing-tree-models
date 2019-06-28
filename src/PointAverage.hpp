#pragma once

#include "Point.hpp"

class PointAverage {
public:
  Point average{};
  float count{};

  void update(Point point);
};
