#include <iostream>

#include "Environment.hpp"
#include "OpenGlWindow.hpp"
#include "Tree.hpp"

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW." << '\n';
    return 1;
  }
  MarkerSet markerSet(2.0f, 10, 1000 * 1000);
  Environment environment(markerSet);
  Tree tree(environment, Point{});
  OpenGlWindow openGlWindow;
  while (!openGlWindow.shouldClose()) {
    openGlWindow.startDrawing();
    openGlWindow.drawTree(environment, tree);
    if (tree.countMetamers() < 100 * 1000) {
      tree.performGrowthIteration();
    }
    openGlWindow.swapBuffers();
    openGlWindow.pollEvents();
  }
  std::cout << "Metamers: " << tree.countMetamers() << '\n';
  glfwTerminate();
  return 0;
}
