#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>

class Image {
  cv::Mat image;

public:
  Image(std::vector<uint8_t> array, const uint32_t width, const uint32_t height);

  void writeToFile(const std::string &filename) const;
};
