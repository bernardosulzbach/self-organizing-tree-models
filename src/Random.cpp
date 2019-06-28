#include "Random.hpp"

constexpr SplitMixGenerator::Result SplitMixGenerator::min() {
  return 0;
}

constexpr SplitMixGenerator::Result SplitMixGenerator::max() {
  return UINT32_MAX;
}

bool operator==(SplitMixGenerator const &lhs, SplitMixGenerator const &rhs) {
  return lhs.m_seed == rhs.m_seed;
}

bool operator!=(SplitMixGenerator const &lhs, SplitMixGenerator const &rhs) {
  return lhs.m_seed != rhs.m_seed;
}

SplitMixGenerator::SplitMixGenerator() : m_seed(1) {
}

SplitMixGenerator::SplitMixGenerator(std::random_device &rd) {
  seed(rd);
}

void SplitMixGenerator::seed(std::random_device &rd) {
  m_seed = uint64_t(rd()) << 31u | uint64_t(rd());
}

SplitMixGenerator::Result SplitMixGenerator::next() {
  uint64_t z = (m_seed += UINT64_C(0x9E3779B97F4A7C15));
  z = (z ^ (z >> 30u)) * UINT64_C(0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27u)) * UINT64_C(0x94D049BB133111EB);
  return Result((z ^ (z >> 31u)) >> 31u);
}

float SplitMixGenerator::nextUniformInRange(float a, float b) {
  const auto alpha = next() / static_cast<float>((max() - min()));
  return a + (b - a) * alpha;
}

double SplitMixGenerator::nextUniformInRange(double a, double b) {
  const auto alpha = next() / static_cast<double>((max() - min()));
  return a + (b - a) * alpha;
}

void SplitMixGenerator::discard(unsigned long long n) {
  for (unsigned long long i = 0; i < n; ++i) {
    next();
  }
}
