#include "OpenGlWindow.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Color.hpp"
#include "Text.hpp"

constexpr uint32_t CylinderFaces = 16;
constexpr uint32_t DefaultWindowSide = 800;
constexpr uint32_t MultiSamplingSamples = 4;

static void testCompilation(const std::string &filename, GLuint shader) {
  GLint isCompiled = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes space for the the string terminator.
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    // Provide the log in whatever manor you deem best.
    std::cerr << std::string(std::begin(errorLog), std::end(errorLog)) << '\n';
    // Don't leak the shader.
    glDeleteShader(shader);
    // Exit with failure.
    throw std::runtime_error("Compilation of shader " + filename + " failed.");
  }
}

GLuint loadShader(const std::string &filename, GLint type) {
  std::string vertexShaderSource = readFileContent(filename);
  const auto begin = std::begin(vertexShaderSource);
  const auto end = std::end(vertexShaderSource);
  std::vector<GLchar> vertexShaderSourceArray(begin, end);
  vertexShaderSourceArray.push_back('\0');
  const auto shader = glCreateShader(type);
  const auto *sourcePointer = vertexShaderSourceArray.data();
  glShaderSource(shader, 1, &sourcePointer, nullptr);
  glCompileShader(shader);
  testCompilation(filename, shader);
  return shader;
}

void assertNoError() {
  if (glGetError() != GL_NO_ERROR) {
    std::cerr << "OpenGL assertion failed." << '\n';
    std::exit(EXIT_FAILURE);
  }
}

GLuint linkProgram(const std::vector<GLuint> &shaders) {
  const auto program = glCreateProgram();
  for (const auto shader : shaders) {
    glAttachShader(program, shader);
  }
  glLinkProgram(program);
  GLint linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    char *infoLog = new char[infoLogLength];
    glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog);
    std::cout << std::string(infoLog) << '\n';
    delete[] infoLog;
    throw std::runtime_error("Program linking failed.");
  }
  assertNoError();
  return program;
}

void OpenGlWindow::initializePrograms() {
  const auto vertexShaderPath = "../shaders/OpenGlVertexShader.glsl";
  const auto vertexShader = loadShader(vertexShaderPath, GL_VERTEX_SHADER);
  const auto fragmentShaderPath = "../shaders/OpenGlFragmentShader.glsl";
  const auto fragmentShader = loadShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
  openGlCylinderProgram = linkProgram({vertexShader, fragmentShader});
  glUseProgram(openGlCylinderProgram);
  const auto vertexColorUniform = glGetUniformLocation(openGlCylinderProgram, "vertexColor");
  openGlCylinderProgramVertexColorUniformLocation = vertexColorUniform;
  const auto modelUniform = glGetUniformLocation(openGlCylinderProgram, "modelMatrix");
  openGlCylinderProgramModelMatrixUniformLocation = modelUniform;
  const auto viewUniform = glGetUniformLocation(openGlCylinderProgram, "viewMatrix");
  openGlCylinderProgramViewMatrixUniformLocation = viewUniform;
  const auto projectionUniform = glGetUniformLocation(openGlCylinderProgram, "projectionMatrix");
  openGlCylinderProgramProjectionMatrixUniformLocation = projectionUniform;
}

static void addDiskVertices(std::vector<float> &diskVertices, float y) {
  for (uint32_t i = 0; i < CylinderFaces; i++) {
    diskVertices.push_back(0.0f);
    diskVertices.push_back(y);
    diskVertices.push_back(0.0f);
    const auto thetaA = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * i;
    diskVertices.push_back(std::cos(thetaA));
    diskVertices.push_back(y);
    diskVertices.push_back(std::sin(thetaA));
    const auto thetaB = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * (i + 1);
    diskVertices.push_back(std::cos(thetaB));
    diskVertices.push_back(y);
    diskVertices.push_back(std::sin(thetaB));
  }
}

static void addSideVertices(std::vector<float> &diskVertices) {
  // Adds two triangles per iteration.
  for (uint32_t i = 0; i < CylinderFaces; i++) {
    const auto thetaA = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * i;
    diskVertices.push_back(std::cos(thetaA));
    diskVertices.push_back(-1.0f);
    diskVertices.push_back(std::sin(thetaA));
    const auto thetaB = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * (i + 1);
    diskVertices.push_back(std::cos(thetaB));
    diskVertices.push_back(-1.0f);
    diskVertices.push_back(std::sin(thetaB));
    diskVertices.push_back(std::cos(thetaB));
    diskVertices.push_back(+1.0f);
    diskVertices.push_back(std::sin(thetaB));

    diskVertices.push_back(std::cos(thetaB));
    diskVertices.push_back(+1.0f);
    diskVertices.push_back(std::sin(thetaB));
    diskVertices.push_back(std::cos(thetaA));
    diskVertices.push_back(+1.0f);
    diskVertices.push_back(std::sin(thetaA));
    diskVertices.push_back(std::cos(thetaA));
    diskVertices.push_back(-1.0f);
    diskVertices.push_back(std::sin(thetaA));
  }
}

void OpenGlWindow::setUpVertexArrays() {
  glGenVertexArrays(1, &openGlCylinderVertexBufferArray);
  glBindVertexArray(openGlCylinderVertexBufferArray);
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  std::vector<float> cylinderVertices;
  addDiskVertices(cylinderVertices, -1.0f);
  addSideVertices(cylinderVertices);
  addDiskVertices(cylinderVertices, +1.0f);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cylinderVertices.size(), cylinderVertices.data(), GL_STATIC_DRAW);
  const auto vertexPositionLocation = glGetAttribLocation(openGlCylinderProgram, "vertexPosition");
  if (vertexPositionLocation < 0) {
    throw std::runtime_error("Could not find an attribute in the OpenGL program.");
  }
  glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)(0 * sizeof(float)));
  glEnableVertexAttribArray(vertexPositionLocation);
  glBindVertexArray(0);
  glDeleteBuffers(1, &vertexBuffer);
}

/**
 * Returns the matrix required to align A with B.
 *
 * Uses the formulation proposed in https://math.stackexchange.com/questions/180418/.
 */
static glm::mat4 getAlignmentMatrix(Vector a, Vector b) {
  a = a.normalize();
  b = b.normalize();
  const auto c = a.dot(b);
  // If the vector is almost aligned or reversed, do nothing.
  if (std::abs(c) > (1.0f - 1.0e-6f)) {
    return glm::mat4(1.0f);
  }
  const auto v = a.cross(b);
  auto skewSymmetric = glm::mat4(0.0f);
  skewSymmetric[0][1] = +v.z;
  skewSymmetric[0][2] = -v.y;
  skewSymmetric[1][0] = -v.z;
  skewSymmetric[1][2] = +v.x;
  skewSymmetric[2][0] = +v.y;
  skewSymmetric[2][1] = -v.x;
  return glm::mat4(1.0f) + skewSymmetric + skewSymmetric * skewSymmetric / (1.0f + c);
}

glm::mat4 modelMatrixFromMetamer(const std::unique_ptr<Metamer> &metamer) {
  // The cylinder is 2 meters high and has 1 meter radius. Its center is at the origin.
  // Scale it on Y to get the right length.
  const auto yScale = Environment::MetamerLength / 2.0f;
  const auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(metamer->width, yScale, metamer->width));
  // Rotate it so that the orientation is correct.
  const auto rotation = getAlignmentMatrix(Vector(0.0f, 1.0f, 0.0f), Vector(metamer->beginning, metamer->end));
  // Translate it so that the centers match.
  const auto metamerCenter = metamer->getCenter();
  const auto center = glm::vec3(metamerCenter.x, metamerCenter.y, metamerCenter.z);
  const auto translation = glm::translate(glm::mat4(1.0f), center);
  return translation * rotation * scale;
}

void OpenGlWindow::drawMetamers(const std::unique_ptr<Metamer> &metamer) {
  if (!metamer) {
    return;
  }
  const auto transform = modelMatrixFromMetamer(metamer);
  glUniformMatrix4fv(openGlCylinderProgramModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(transform));
  glDrawArrays(GL_TRIANGLES, 0, 3 * (4 * CylinderFaces));
  drawCalls++;
  drawMetamers(metamer->axillary);
  drawMetamers(metamer->terminal);
}

OpenGlWindow::OpenGlWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_SAMPLES, MultiSamplingSamples);
  window = glfwCreateWindow(DefaultWindowSide, DefaultWindowSide, "OpenGL Window", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);
  glEnable(GL_MULTISAMPLE);
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create the OpenGL window.");
  }
  initializePrograms();
  setUpVertexArrays();
}

OpenGlWindow::~OpenGlWindow() {
  glfwDestroyWindow(window);
}

void OpenGlWindow::setBoundingBox(BoundingBox box) {
  boundingBox = box;
}

void OpenGlWindow::drawTree(const Environment &environment, const Tree &tree) {
  const auto cameraPosition = glm::vec3(0.0f, 0.25f, 0.75f);
  const auto lookAtPosition = glm::vec3(0.0f, 0.25f, 0.0f);
  const auto viewMatrix = glm::lookAt(cameraPosition, lookAtPosition, glm::vec3(0.0f, 1.0f, 0.0f));
  const auto fov = 2.0f * std::atan(1.0f);
  const auto ratio = 1.0f;
  const auto ZNear = 0.1f;
  const auto zFar = 100.0f;
  const auto projectionMatrix = glm::perspective(fov, ratio, ZNear, zFar);
  glUseProgram(openGlCylinderProgram);
  glBindVertexArray(openGlCylinderVertexBufferArray);
  const auto viewPointer = glm::value_ptr(viewMatrix);
  glUniformMatrix4fv(openGlCylinderProgramViewMatrixUniformLocation, 1, GL_FALSE, viewPointer);
  const auto projectionPointer = glm::value_ptr(projectionMatrix);
  glUniformMatrix4fv(openGlCylinderProgramProjectionMatrixUniformLocation, 1, GL_FALSE, projectionPointer);
  // Olive Wood
  const Color color{0.4588f, 0.3843f, 0.2667f};
  glUniform4fv(openGlCylinderProgramVertexColorUniformLocation, 1, color.channels.data());
  drawMetamers(tree.root);
}

bool OpenGlWindow::shouldClose() {
  return glfwWindowShouldClose(window);
}

void OpenGlWindow::startDrawing() {
  int width;
  int height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawCalls = 0;
}

void OpenGlWindow::swapBuffers() {
  std::cout << "Draw calls: " << drawCalls << '\n';
  glfwSwapBuffers(window);
}

void OpenGlWindow::pollEvents() {
  glfwPollEvents();
}
