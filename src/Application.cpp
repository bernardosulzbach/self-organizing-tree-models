#include <iomanip>
#include <iostream>
#include <optional>

#include "Environment.hpp"
#include "Image.hpp"
#include "OpenGlWindow.hpp"
#include "Random.hpp"
#include "Tree.hpp"

enum class Mode { Standard, Image, Video };

static constexpr U32 TargetMetamers = 5 * 1000;

void saveFramebuffer(const std::string &filename) {
  std::vector<uint8_t> imageData(OpenGlWindow::DefaultWindowSide * OpenGlWindow::DefaultWindowSide * 3);
  glReadBuffer(GL_BACK);
  glReadPixels(0, 0, OpenGlWindow::DefaultWindowSide, OpenGlWindow::DefaultWindowSide, GL_RGB, GL_UNSIGNED_BYTE, imageData.data());
  std::reverse(std::begin(imageData), std::end(imageData));
  Image image(imageData, OpenGlWindow::DefaultWindowSide, OpenGlWindow::DefaultWindowSide);
  image.writeToFile(filename);
}

int main(int argc, char *argv[]) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW." << '\n';
    return 1;
  }
  Mode mode = Mode::Standard;
  std::optional<BoundingBox> userSpecifiedBoundingBox;
  for (int i = 0; i < argc; i++) {
    const auto argument = std::string(argv[i]);
    if (argument == "--image") {
      mode = Mode::Image;
    } else if (argument == "--video") {
      mode = Mode::Video;
    } else if (argument == "--bounding-box") {
      std::stringstream values;
      for (int j = 0; j < 6; j++) {
        i++;
        if (j != 0) {
          values << ' ';
        }
        values << argv[i];
      }
      userSpecifiedBoundingBox = BoundingBox(values.str());
    }
  }
  const auto begin = std::chrono::steady_clock::now();
  SplitMixGenerator splitMixGenerator;
  MarkerSet markerSet(splitMixGenerator, 2.0f, 10, 1000 * 1000);
  Environment environment(splitMixGenerator, markerSet);
  Tree tree(environment, Point{});
  OpenGlWindow openGlWindow;
  U64 frameIndex = 0;
  while (!openGlWindow.shouldClose()) {
    const auto metamerCount = tree.countMetamers();
    openGlWindow.startDrawing();
    if (userSpecifiedBoundingBox) {
      openGlWindow.setCameraForBoundingBox(userSpecifiedBoundingBox.value());
    } else {
      openGlWindow.setCameraForBoundingBox(tree.getBoundingBox());
    }
    if (mode == Mode::Standard || mode == Mode::Video) {
      openGlWindow.drawTree(tree);
    }
    if (metamerCount < TargetMetamers) {
      tree.performGrowthIteration();
    } else {
      if (mode == Mode::Image) {
        openGlWindow.drawTree(tree);
        const std::chrono::duration<float> duration = std::chrono::steady_clock::now() - begin;
        std::cout << "Duration: " << std::setprecision(3) << duration.count() << " s" << '\n';
        saveFramebuffer("image.png");
      }
      if (mode == Mode::Image || mode == Mode::Video) {
        openGlWindow.setShouldClose();
      }
    }
    if (mode == Mode::Video) {
      frameIndex++;
      std::stringstream frameNumber;
      frameNumber << std::setfill('0') << std::setw(5) << frameIndex;
      saveFramebuffer("video/frame-" + frameNumber.str() + ".png");
    }
    openGlWindow.swapBuffers();
    openGlWindow.pollEvents();
  }
  std::cout << "Tree bounding box: " << tree.getBoundingBox().toString() << '\n';
  std::cout << "Metamers: " << tree.countMetamers() << '\n';
  glfwTerminate();
  return 0;
}
