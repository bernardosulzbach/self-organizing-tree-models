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

static UserAction getUserActionFromKey(int key) {
  switch (key) {
  case GLFW_KEY_W:
    return UserAction::MoveCloser;
  case GLFW_KEY_S:
    return UserAction::MoveAway;
  default:
    return UserAction::None;
  }
}

static std::array<bool, static_cast<U32>(UserAction::Count)> userActions;

void keyCallback(GLFWwindow *, int key, int, int action, int) {
  if (action == GLFW_PRESS || action == GLFW_RELEASE) {
    userActions[userActionToIndex(getUserActionFromKey(key))] = (action == GLFW_PRESS);
  }
}

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
  const auto modelInverseTransposedUniform = glGetUniformLocation(openGlCylinderProgram, "modelInverseTransposedMatrix");
  openGlCylinderProgramModelInverseTransposedMatrixUniformLocation = modelInverseTransposedUniform;
  const auto viewUniform = glGetUniformLocation(openGlCylinderProgram, "viewMatrix");
  openGlCylinderProgramViewMatrixUniformLocation = viewUniform;
  const auto projectionUniform = glGetUniformLocation(openGlCylinderProgram, "projectionMatrix");
  openGlCylinderProgramProjectionMatrixUniformLocation = projectionUniform;
  const auto cameraPositionUniform = glGetUniformLocation(openGlCylinderProgram, "cameraPositionInWorld");
  openGlCylinderProgramCameraPositionInWorldUniformLocation = cameraPositionUniform;
  const auto lightPositionUniform = glGetUniformLocation(openGlCylinderProgram, "lightPositionInWorld");
  openGlCylinderProgramLightPositionInWorldUniformLocation = lightPositionUniform;
  const auto ambientLightIntensityUniform = glGetUniformLocation(openGlCylinderProgram, "ambientLightIntensity");
  openGlCylinderProgramAmbientLightIntensityUniformLocation = ambientLightIntensityUniform;
}

static void pushBackValues(std::vector<float> &values, float x, float y, float z) {
  values.push_back(x);
  values.push_back(y);
  values.push_back(z);
}

static void addDiskVertices(std::vector<float> &values, float y, float ny) {
  for (uint32_t i = 0; i < CylinderFaces; i++) {
    pushBackValues(values, 0.0f, y, 0.0f);
    pushBackValues(values, 0.0f, ny, 0.0f);
    const auto thetaA = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * i;
    pushBackValues(values, std::cos(thetaA), y, std::sin(thetaA));
    pushBackValues(values, 0.0f, ny, 0.0f);
    const auto thetaB = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * (i + 1);
    pushBackValues(values, std::cos(thetaB), y, std::sin(thetaB));
    pushBackValues(values, 0.0f, ny, 0.0f);
  }
}

static void addSideFaces(std::vector<float> &values, const float thetaA, const float thetaB) {
  // Add the first triangle.
  pushBackValues(values, std::cos(thetaA), -1.0f, std::sin(thetaA));
  pushBackValues(values, std::cos(thetaA), 0.0f, std::sin(thetaA));
  pushBackValues(values, std::cos(thetaB), -1.0f, std::sin(thetaB));
  pushBackValues(values, std::cos(thetaB), 0.0f, std::sin(thetaB));
  pushBackValues(values, std::cos(thetaB), 1.0f, std::sin(thetaB));
  pushBackValues(values, std::cos(thetaB), 0.0f, std::sin(thetaB));
  // Add the second triangle.
  pushBackValues(values, std::cos(thetaB), 1.0f, std::sin(thetaB));
  pushBackValues(values, std::cos(thetaB), 0.0f, std::sin(thetaB));
  pushBackValues(values, std::cos(thetaA), 1.0f, std::sin(thetaA));
  pushBackValues(values, std::cos(thetaA), 0.0f, std::sin(thetaA));
  pushBackValues(values, std::cos(thetaA), -1.0f, std::sin(thetaA));
  pushBackValues(values, std::cos(thetaA), 0.0f, std::sin(thetaA));
}

static void addSideVertices(std::vector<float> &values) {
  // Adds two triangles per iteration.
  for (uint32_t i = 0; i < CylinderFaces; i++) {
    const auto thetaA = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * i;
    const auto thetaB = 2.0f * 4.0f * std::atan(1.0f) / CylinderFaces * (i + 1);
    addSideFaces(values, thetaA, thetaB);
  }
}

void OpenGlWindow::setUpVertexArrays() {
  glGenVertexArrays(1, &openGlCylinderVertexBufferArray);
  glBindVertexArray(openGlCylinderVertexBufferArray);
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  std::vector<float> cylinderValues;
  addDiskVertices(cylinderValues, -1.0f, -1.0f);
  addSideVertices(cylinderValues);
  addDiskVertices(cylinderValues, +1.0f, +1.0f);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cylinderValues.size(), cylinderValues.data(), GL_STATIC_DRAW);
  const auto vertexPositionLocation = glGetAttribLocation(openGlCylinderProgram, "vertexPosition");
  if (vertexPositionLocation < 0) {
    throw std::runtime_error("Could not find vertexPosition in the OpenGL program.");
  }
  glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(0 * sizeof(float)));
  glEnableVertexAttribArray(vertexPositionLocation);
  const auto vertexNormalLocation = glGetAttribLocation(openGlCylinderProgram, "vertexNormal");
  if (vertexNormalLocation < 0) {
    throw std::runtime_error("Could not find vertexNormal in the OpenGL program.");
  }
  glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(vertexNormalLocation);
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
  const auto modelMatrix = modelMatrixFromMetamer(metamer);
  glUniformMatrix4fv(openGlCylinderProgramModelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
  const auto modelInverseTransposedMatrix = glm::transpose(glm::inverse(modelMatrix));
  glUniformMatrix4fv(openGlCylinderProgramModelInverseTransposedMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelInverseTransposedMatrix));
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
  glfwSetKeyCallback(window, keyCallback);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);
  glEnable(GL_MULTISAMPLE);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create the OpenGL window.");
  }
  initializePrograms();
  setUpVertexArrays();
}

OpenGlWindow::~OpenGlWindow() {
  glfwDestroyWindow(window);
}

void OpenGlWindow::drawTree(const Environment &environment, const Tree &tree) {
  const auto glmCameraPosition = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
  const auto lookAtPosition = glm::vec3(0.0f, 0.5f, 0.0f);
  const auto viewMatrix = glm::lookAt(glmCameraPosition, lookAtPosition, glm::vec3(0.0f, 1.0f, 0.0f));
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
  const auto cameraPositionInWorldPointer = glm::value_ptr(glmCameraPosition);
  glUniform3fv(openGlCylinderProgramCameraPositionInWorldUniformLocation, 1, cameraPositionInWorldPointer);
  const auto glmLightPosition = glmCameraPosition + glm::vec3(0.0f, 1.0f, 0.0f);
  const auto lightPositionInWorldPointer = glm::value_ptr(glmLightPosition);
  glUniform3fv(openGlCylinderProgramLightPositionInWorldUniformLocation, 1, lightPositionInWorldPointer);
  glUniform1f(openGlCylinderProgramAmbientLightIntensityUniformLocation, 1.0f);
  // Olive Wood
  const Color color{0.4588f, 0.3843f, 0.2667f};
  glUniform4fv(openGlCylinderProgramVertexColorUniformLocation, 1, color.channels.data());
  drawMetamers(tree.root);
}

void OpenGlWindow::setShouldClose() {
  glfwSetWindowShouldClose(window, true);
}

bool OpenGlWindow::shouldClose() {
  return glfwWindowShouldClose(window);
}

void OpenGlWindow::updateCameraPosition() {
  const auto currentTime = std::chrono::steady_clock::now();
  const std::chrono::duration<float> seconds = currentTime - lastUpdate;
  auto newCameraPosition = cameraPosition;
  const auto approximationFactor = std::pow(2.0f, seconds.count());
  if (userActions[userActionToIndex(UserAction::MoveCloser)]) {
    Vector vector(cameraPosition, Point(0.0f, 0.0f, 0.0f));
    vector = vector.scale((approximationFactor - 1.0f) / approximationFactor);
    newCameraPosition = newCameraPosition.translate(vector.x, vector.y, vector.z);
  }
  if (userActions[userActionToIndex(UserAction::MoveAway)]) {
    Vector vector(Point(0.0f, 0.0f, 0.0f), cameraPosition);
    vector = vector.scale(approximationFactor - 1.0f);
    newCameraPosition = newCameraPosition.translate(vector.x, vector.y, vector.z);
  }
  lastUpdate = currentTime;
  cameraPosition = newCameraPosition;
}

void OpenGlWindow::startDrawing() {
  int width;
  int height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  updateCameraPosition();
  drawCalls = 0;
}

void OpenGlWindow::swapBuffers() {
  std::cout << "Draw calls: " << drawCalls << '\n';
  glfwSwapBuffers(window);
}

void OpenGlWindow::pollEvents() {
  glfwPollEvents();
}
