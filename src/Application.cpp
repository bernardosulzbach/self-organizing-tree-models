#include <iostream>

#include "Environment.hpp"
#include "OpenGlWindow.hpp"
#include "Tree.hpp"

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW." << '\n';
    return 1;
  }
  Environment environment;
  Tree tree(environment, Point{});
  OpenGlWindow openGlWindow;
  uint32_t growthIterationsRemaining = 512;
  while (!openGlWindow.shouldClose()) {
    openGlWindow.startDrawing();
    openGlWindow.drawTree(environment, tree);
    if (growthIterationsRemaining > 0) {
      tree.performGrowthIteration();
      growthIterationsRemaining--;
    }
    openGlWindow.swapBuffers();
    openGlWindow.pollEvents();
  }
  std::cout << "Metamers: " << tree.countMetamers() << '\n';
  glfwTerminate();
  return 0;
}
