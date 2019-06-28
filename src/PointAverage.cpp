#include "PointAverage.hpp"

void PointAverage::update(Point point) {
  const auto alpha = count / (count + 1.0f);
  average.x = alpha * average.x + (1.0f - alpha) * point.x;
  average.y = alpha * average.y + (1.0f - alpha) * point.y;
  average.z = alpha * average.z + (1.0f - alpha) * point.z;
  count += 1.0f;
}
