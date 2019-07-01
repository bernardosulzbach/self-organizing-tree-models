#include <iostream>

#include "Environment.hpp"
#include "OpenGlWindow.hpp"
#include "Random.hpp"
#include "Tree.hpp"

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW." << '\n';
    return 1;
  }
  SplitMixGenerator splitMixGenerator;
  MarkerSet markerSet(splitMixGenerator, 2.0f, 10, 1000 * 1000);
  Environment environment(splitMixGenerator, markerSet);
  Tree tree(environment, Point{});
  OpenGlWindow openGlWindow;
  while (!openGlWindow.shouldClose()) {
    openGlWindow.startDrawing();
    openGlWindow.drawTree(tree);
    if (tree.countMetamers() < 5 * 1000) {
      tree.performGrowthIteration();
    } else {
      // openGlWindow.setShouldClose();
    }
    openGlWindow.swapBuffers();
    openGlWindow.pollEvents();
  }
  std::cout << "Metamers: " << tree.countMetamers() << '\n';
  glfwTerminate();
  return 0;
}
