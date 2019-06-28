#include "Text.hpp"

#include <fstream>
#include <string>

std::string readFileContent(const std::string &filename) {
  std::ifstream stream(filename);
  if (stream.fail()) {
    throw std::runtime_error("Could not open " + filename + ".");
  }
  const auto &begin = std::istreambuf_iterator<char>(stream);
  return std::string(begin, std::istreambuf_iterator<char>());
}
