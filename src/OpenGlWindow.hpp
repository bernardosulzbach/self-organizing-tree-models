#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "BoundingBox.hpp"
#include "Color.hpp"
#include "Tree.hpp"
#include "Types.hpp"

class OpenGlWindow {
  GLFWwindow *window = nullptr;

  GLuint openGlCylinderProgram = -1;
  GLuint openGlCylinderVertexBufferArray = -1;
  GLuint openGlCylinderProgramVertexColorUniformLocation = -1;
  GLuint openGlCylinderProgramModelMatrixUniformLocation = -1;
  GLuint openGlCylinderProgramProjectionMatrixUniformLocation = -1;
  GLuint openGlCylinderProgramViewMatrixUniformLocation = -1;

  BoundingBox boundingBox;

  U64 drawCalls = 0;

  void initializePrograms();

  void setUpVertexArrays();

  void drawMetamers(const std::unique_ptr<Metamer> &metamer);

public:
  OpenGlWindow();

  virtual ~OpenGlWindow();

  void setBoundingBox(BoundingBox box);

  void drawTree(const Environment &environment, const Tree &tree);

  bool shouldClose();

  void startDrawing();

  void pollEvents();

  void swapBuffers();
};
