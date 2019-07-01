#include "Image.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

Image::Image(std::vector<uint8_t> array, const uint32_t width, const uint32_t height) {
  if (array.size() != width * height * 3) {
    // TODO: test this.
    const auto expectedValues = std::to_string(width * height * 3);
    const auto actualValues = std::to_string(array.size());
    throw std::runtime_error("Expected " + expectedValues + " values, but got " + actualValues + ".");
  }
  // Make an OpenCV image here.
  image = cv::Mat(height, width, CV_8UC3);
  for (int32_t i = 0; i < image.rows; i++) {
    for (int32_t j = 0; j < image.cols; j++) {
      for (int32_t k = 0; k < 3; k++) {
        image.at<cv::Vec3b>(i, j)[k] = array[(i * image.cols + j) * 3 + k];
      }
    }
  }
}

void Image::writeToFile(const std::string &filename) const {
  cv::imwrite(filename, image);
}
