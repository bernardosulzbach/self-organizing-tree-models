#pragma once

#include <cstdint>
#include <random>

/**
 * Copyright (c) 2018 Arvid Gerstmann.
 *
 * Licensed under the MIT license.
 *
 * Adapted by Bernardo Sulzbach in 2019.
 */
class SplitMixGenerator {
public:
  using Result = uint32_t;

  static constexpr Result min();

  static constexpr Result max();

  friend bool operator==(SplitMixGenerator const &, SplitMixGenerator const &);
  friend bool operator!=(SplitMixGenerator const &, SplitMixGenerator const &);

  SplitMixGenerator();

  explicit SplitMixGenerator(std::random_device &rd);

  void seed(std::random_device &rd);

  Result next();

  float nextUniformInRange(float a, float b);

  double nextUniformInRange(double a, double b);

  void discard(unsigned long long n);

private:
  uint64_t m_seed;
};
