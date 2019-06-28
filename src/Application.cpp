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
  while (!openGlWindow.shouldClose()) {
    openGlWindow.startDrawing();
    openGlWindow.drawTree(environment, tree);
    openGlWindow.swapBuffers();
    openGlWindow.pollEvents();
  }
  glfwTerminate();
  return 0;
}
