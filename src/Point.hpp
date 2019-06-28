#pragma once

class Point {
public:
  float x{};
  float y{};
  float z{};

  Point() = default;

  Point(float x, float y, float z);

  Point translate(float dx, float dy, float dz);
};
